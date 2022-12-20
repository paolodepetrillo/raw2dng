FROM alpine:latest AS builder
RUN apk add --update --no-cache libraw-dev exiv2-dev expat-dev jpeg-dev zlib-dev build-base cmake && \
    mkdir -p /usr/src/raw2dng
WORKDIR /usr/src/raw2dng
COPY . .
RUN cmake . && make

FROM alpine:latest
RUN apk add --update --no-cache libraw exiv2 expat jpeg zlib
COPY --from=builder /usr/src/raw2dng/raw2dng/raw2dng /usr/local/bin/raw2dng
CMD raw2dng
