FROM ubuntu:18.04 
#ENV.UTF-8 
RUN apt-get update -y 
RUN apt-get -y install gcc \
	g++ \     
	gdb \     
	automake \     
	autoconf \     
	libtool \    
	make \     
	cmake 
RUN apt-get -y install ssh \     
	ntp \     
	vim \     
	wget \     
	curl \     
	telnet 
RUN apt-get -y install git \     
	subversion \     
	doxygen \     
	lighttpd \     
	net-tools \     
	inetutils-ping 
RUN apt-get -y install python \     
	golang
RUN apt-get -y install libbz2-dev \     
	libdb++-dev \     
	libssl-dev \     
	libdb-dev \     
	libssl-dev \     
	openssl \     
	libreadline-dev \     
	libcurl4-openssl-dev \     
	libncurses-dev \     
	autotools-dev \     
	build-essential \     
	libicu-dev \     
	python-dev 
RUN mkdir /var/run/sshd 
# change sshd listen port 
RUN sed -i 's/Port[ ]*22/Port 36099/' /etc/ssh/sshd_config 
RUN echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config 
# add user 
RUN mkdir /home/zhanggan 
RUN useradd -s /bin/bash zhanggan 
RUN echo "zhanggan:Gutfee2ing" | chpasswd 
RUN echo "zhanggan:Gutfee2ing" | chpasswd 
RUN chown -R zhanggan:zhanggan /home/zhanggan 
# Container should expose ports. 
EXPOSE 36099 CMD ["/usr/sbin/sshd", "-D"]

