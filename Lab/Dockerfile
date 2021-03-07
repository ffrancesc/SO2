FROM ubuntu:xenial

RUN apt-get update

# Needed tools to compile bochs...
RUN apt-get -y install \
    build-essential \
    libx11-dev \
    libgtk2.0-dev \
    libreadline-dev \
    wget

# Needed tools for Zeos
RUN apt-get -y install vim bin86 gdb git sudo

RUN mkdir bochs-tmp
WORKDIR /bochs-tmp

# Download BOCHS
ENV VERS=2.6.7
ENV BOCHS=bochs-${VERS}
RUN wget http://sourceforge.net/projects/bochs/files/bochs/${VERS}/${BOCHS}.tar.gz/download -O ${BOCHS}.tar.gz && \
    tar zxf ${BOCHS}.tar.gz

# Compile BOCHS...
# ... with GDB
RUN mkdir build-gdb && \
    cd build-gdb && \
    LDFLAGS=-pthread ../${BOCHS}/configure --enable-gdb-stub --with-x --prefix=/opt/bochs_gdb && \
    make && \ 
    make install && \
    cd ..

# ... without GDB
RUN mkdir build && \
    cd build && \
    LDFLAGS=-pthread ../${BOCHS}/configure --enable-debugger --enable-disasm --enable-x86-debugger --enable-readline --with-x --prefix=/opt/bochs && \
    make && \
    make install && \
    cd ..

# Create links to bochs binaries
RUN ln -sf /opt/bochs/bin/bochs /usr/local/bin/bochs_nogdb && \
    ln -sf /opt/bochs_gdb/bin/bochs /usr/local/bin/bochs

# Clean environment
RUN rm -rf /bochs-tmp

WORKDIR /workspaces/SO2/Lab