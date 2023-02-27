FROM gcc:12

RUN apt-get update && \
    apt-get install -y libssl-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY main.cpp .

COPY app_modules /app/app_modules

RUN g++ -std=c++17 main.cpp -o fileserver -lssl -lcrypto

RUN rm -rf app_modules main.cpp
