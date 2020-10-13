FROM faasm/toolchain:10.0.1

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

# Build eigen
RUN inv eigen

