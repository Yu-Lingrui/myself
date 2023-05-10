FROM PACKAGE_BASE_IMAGE

COPY . /usr/local/ev_sdk


RUN cd /usr/local/ev_sdk \
	&& cd /usr/local/ev_sdk && rm -rf build \
	&& mkdir -p build && cd build \
	&& cmake -DCMAKE_BUILD_TYPE=release .. && make -j4 && make install \
	&& cd /usr/local/ev_sdk/test && rm -rf build && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=release .. && make -j4 && make install
	
ENV LD_LIBRARY_PATH=/usr/local/ev_sdk/lib/:$LD_LIBRARY_PATH




