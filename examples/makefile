default: libutil.so
	make -C backAdvect $@; \
	make -C bilateral $@; \
	make -C convolution $@; \
	make -C convolution_separable $@; \
	make -C canny $@; \
	make -C hough $@; \
	make -C demosaic $@; \
	make -C diff $@; \
	make -C lucaskanade $@; \
	make -C motionestimation $@; \
	make -C rotate $@; \
	make -C trim $@; \
	make -C lineDetection $@; \
	make -C label $@; \
	make -C text $@; \

debug: libutil.so
	make -C backAdvect $@; \
	make -C bilateral $@; \
	make -C convolution $@; \
	make -C convolution_separable $@; \
	make -C canny $@; \
	make -C hough $@; \
	make -C demosaic $@; \
	make -C diff $@; \
	make -C lucaskanade $@; \
	make -C motionestimation $@; \
	make -C rotate $@; \
	make -C lineDetection $@; \
	make -C label $@; \
	make -C text $@; \

clean:
	make -C backAdvect $@; \
	make -C bilateral $@; \
	make -C convolution $@; \
	make -C convolution_separable $@; \
	make -C canny $@; \
	make -C hough $@; \
	make -C demosaic $@; \
	make -C diff $@; \
	make -C lucaskanade $@; \
	make -C motionestimation $@; \
	make -C rotate $@; \
	make -C trim $@; \
	make -C lineDetection $@; \
	make -C label $@; \
	make -C text $@; \

libutil.so: util.cpp
	g++ -g -fPIC -shared util.cpp -o libutil.so