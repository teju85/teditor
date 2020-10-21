FROM ubuntu:20.04
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        curl \
        doxygen \
        git && \
    rm -rf /var/lib/apt/lists/*
COPY run.sh /run.sh
RUN chmod +x /run.sh
ENTRYPOINT ["/run.sh"]
