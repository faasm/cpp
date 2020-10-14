FROM faasm/toolchain:10.0.1

# Install Python tooling
RUN apt update
RUN apt install -y \
    python3-dev \
    python3-pip \
    libtool \
    autotools-dev \
    autoconf

WORKDIR /code
COPY requirements.txt .
RUN pip3 install -r requirements.txt

# Copy the code in
COPY . . 

# Build libraries
RUN inv libc

RUN inv eigen

RUN inv libffi

RUN inv clapack
