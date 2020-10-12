ARG LLVM_VERSION
FROM faasm/toolchain:${LLVM_VERSION}

# Install Python tooling
RUN apt update
RUN apt install -y \
    python3-dev \
    python3-pip 

WORKDIR /code
COPY requirements.txt .
RUN pip3 install -r requirements.txt

# Copy the code in
COPY . . 

# Rebuild libc
RUN inv libc --clean

