FROM ubuntu:latest

ENV TZ=Asia/Seoul LANG=ko_KR.UTF-8 LANGUAGE=ko_KR.UTF-8

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales tzdata build-essential sudo gdb vim curl git wget \
    python3 cmake make gcc sudo valgrind telnet net-tools iproute2\
    && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8\
    # CLion 원격 백엔드와 JetBrains 런타임 의존성용 최소 세트
    apt-get install -y ca-certificates unzip procps \
    libfreetype6 libxi6 libxext6 libxrender1 libxtst6

RUN useradd -m -s /bin/bash jungle && usermod -aG sudo jungle
RUN echo "jungle ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/jungle
# 암호 설정
# RUN echo 'jungle:jungle' | chpasswd

USER jungle
WORKDIR /home/jungle
