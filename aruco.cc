#include <nan.h>
#include <string>
#include <iostream>
#include <aruco/aruco.h>
#include <opencv2/highgui/highgui.hpp>

using namespace v8;
using namespace cv;
using namespace aruco;

NAN_METHOD(Method) {
  info.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

NAN_METHOD(Detect) {
    Nan::HandleScope();

    bool isSync = (info.Length() == 1);

    if ( info.Length() < 1 ) {
        return Nan::ThrowError("detect() requires 1 argument!");
    }

    if ( ! node::Buffer::HasInstance(info[ 0 ]) ) {
        return Nan::ThrowError("detect()'s 1st argument should be a Buffer");
    }

    if( ! isSync && ! info[ 1 ]->IsFunction() ) {
        return Nan::ThrowError("detect()'s 2nd argument should be a function");
    }

    cv::Mat mat;
    uint8_t *buf = (uint8_t *) node::Buffer::Data(info[0]->ToObject());
    unsigned len = node::Buffer::Length(info[0]->ToObject());

    cv::Mat *mbuf = new cv::Mat(len, 1, CV_64FC1, buf);
    mat = cv::imdecode(*mbuf, CV_LOAD_IMAGE_UNCHANGED);

    MarkerDetector MDetector;
    vector<Marker> Markers;
    MDetector.detect(mat, Markers);

    v8::Local<v8::Array> arr = Nan::New<v8::Array>(Markers.size());
    if (Markers.size() != 0){
		for (unsigned int i=0;i<Markers.size();i++) {
        	v8::Local<v8::Object> marker = Nan::New<v8::Object>();
    		Nan::Set(marker, Nan::New("id").ToLocalChecked(), Nan::New(Markers[i].id));
    		v8::Local<v8::Array> points = Nan::New<v8::Array>(4);
    		for (int j = 0; j < 4; j++){
    			v8::Local<v8::Object> point = Nan::New<v8::Object>();
    			Nan::Set(point, Nan::New("x").ToLocalChecked(), Nan::New(Markers[i][j].x));
    			Nan::Set(point, Nan::New("y").ToLocalChecked(), Nan::New(Markers[i][j].y));
    			Nan::Set(points, j, point);
    		}
    		Nan::Set(marker, Nan::New("points").ToLocalChecked(), points);
    		Nan::Set(arr, i, marker);
    	}
    }

    info.GetReturnValue().Set(arr);

    if (mat.empty()) {
      return Nan::ThrowError("Empty Image");
    }

}


NAN_MODULE_INIT(Init) {
  Nan::SetMethod(target, "detect", Detect);
}

NODE_MODULE(aruco, Init)
