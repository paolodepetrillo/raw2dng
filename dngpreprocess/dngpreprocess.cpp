#include <stdexcept>
#include <iostream>
#include <string.h>

#include <dng_camera_profile.h>
#include <dng_file_stream.h>
#include <dng_image_writer.h>
#include <dng_info.h>
#include <dng_negative.h>
#include <dng_xmp_sdk.h>
#include <dnghost.h>

void process(std::string rawFilename, std::string outFilename, std::string dcpFilename)
{
    /* Initialize Adobe DNG SDK */
    dng_xmp_sdk::InitializeSDK();
    dng_host host;

    host.SetSaveDNGVersion(dngVersion_SaveDefault);
    host.SetSaveLinearDNG(false);
    host.SetKeepOriginalFile(false);

    /* Load the input DNG */
    dng_file_stream stream(rawFilename.c_str());
    dng_info info;
    info.Parse(host, stream);
    info.PostParse(host);

    AutoPtr<dng_negative> negative;
    negative.Reset(host.Make_dng_negative());
    negative->Parse(host, stream, info);
    negative->PostParse(host, stream, info);
    negative->ReadStage1Image(host, stream, info);
    negative->ValidateRawImageDigest(host);

    /*
     * Apply stage 1 and stage 2 processing. The black levels are subtracted 
     * from the raw sensor values, and the values are converted to floating 
     * point and scaled to the range 0 to 1. Then, the stage 2 opcodes 
     * including the GainMap are applied. A floating point dng is output, and 
     * the tags corresponding to any operations that were performed here (black 
     * levels, opcodes) are removed.
     */
    negative->OpcodeList2().SetAlwaysApply();
    negative->BuildStage2Image(host);

    /* If a DCP file was specified, replace the input file's camera profile 
     * with the profile from the DCP file.
     */
    if (!dcpFilename.empty()) {
        AutoPtr<dng_camera_profile> prof(new dng_camera_profile);
        dng_file_stream profStream(dcpFilename.c_str());
        if (!prof->ParseExtended(profStream)) {
            throw std::runtime_error("Could not parse supplied camera profile file!");
        }
        negative->ClearProfiles();
        negative->AddProfile(prof);
    }

    dng_file_stream stream_out(outFilename.c_str(), true);
    dng_image_writer writer;
    writer.WriteDNG(host, stream_out, *negative.Get(), NULL, dngVersion_Current, false);
}

int main(int argc, const char* argv []) {  
    if (argc == 1) {
        std::cerr << "\n"
                     "dngpreprocess - DNG preprocessor\n"
                     "Usage: " << argv[0] << " [options] <rawfile>\n"
                     "Valid options:\n"
                     "  -dcp <filename>      use adobe camera profile\n"
                     "  -o <filename>        specify output filename\n\n";
        return -1;
    }

    std::string outFilename;
    std::string dcpFilename;

    int index;
    for (index = 1; index < argc && argv [index][0] == '-'; index++) {
        std::string option = &argv[index][1];
        if (0 == strcmp(option.c_str(), "o"))   outFilename = std::string(argv[++index]);
        if (0 == strcmp(option.c_str(), "dcp")) dcpFilename = std::string(argv[++index]);
    }

    if (index == argc) {
        std::cerr << "No input file specified\n";
        return 1;
    }

    std::string rawFilename(argv[index++]);

    if (outFilename.empty()) {
        outFilename.assign(rawFilename, 0, rawFilename.find_last_of("."));
        outFilename.append("_pp.dng");
    }

    try {
        process(rawFilename, outFilename, dcpFilename);
    }
    catch (std::exception& e) {
        std::cerr << "--> Error! (" << e.what() << ")\n\n";
        return -1;
    }
    catch (dng_exception& e) {
        std::cerr << "--> DNG error code " << e.ErrorCode() << "\n\n";
        return -1;
    }

    return 0;
}
