FROM ubuntu:18.04

RUN mkdir /root/ns3 \
    && cd /root/ns3 \
    && apt update \
    && apt-get install --reinstall ca-certificates -y \
    && apt-get install gcc g++ python git wget cppcheck valgrind gdb make -y --no-install-recommends \
    && wget https://www.nsnam.org/release/ns-allinone-3.29.tar.bz2 \
    && tar xjf ns-allinone-3.29.tar.bz2 \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/* \
    && cd ns-allinone-3.29

WORKDIR /root/ns3/ns-allinone-3.29/ns-3.29/src/minuet
