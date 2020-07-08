FROM ubuntu:18.04
LABEL maintainer="Ilya Popovnin"
LABEL description="ilovecontainers"
RUN apt-get update && apt-get install -y python3
COPY README /README
COPY app /app
WORKDIR /app
CMD cat /README && python3 /app/app.py
