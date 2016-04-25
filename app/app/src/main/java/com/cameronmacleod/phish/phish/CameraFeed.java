package com.cameronmacleod.phish.phish;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.ByteBuffer;
import java.security.Permission;
import java.util.Arrays;

import android.Manifest;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.text.style.BackgroundColorSpan;
import android.util.Base64;
import android.util.Log;
import android.util.Size;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

import org.json.JSONObject;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.CvType;

public class CameraFeed extends Activity {

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_1_0, this, mLoaderCallback);
    }

    private final static String TAG = "SimpleCamera";
    private TextureView mTextureView = null;
    private TextureView.SurfaceTextureListener mSurfaceTextureListner =
            new TextureView.SurfaceTextureListener() {

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
            //Log.i(TAG, "onSurfaceTextureUpdated()");
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,
                                                int height) {
            Log.i(TAG, "onSurfaceTextureSizeChanged()");

        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            Log.i(TAG, "onSurfaceTextureDestroyed()");
            return false;
        }

        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            Log.i(TAG, "onSurfaceTextureAvailable()");

            CameraManager manager = (CameraManager) getSystemService(CAMERA_SERVICE);
            try{
                String cameraId = manager.getCameraIdList()[0];
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
                StreamConfigurationMap map = characteristics.get(
                        CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                mPreviewSize = map.getOutputSizes(SurfaceTexture.class)[0];

                if(checkCameraPermission()) {
                    manager.openCamera(cameraId, mStateCallback, null);
                }
            }
            catch(CameraAccessException e)
            {
                e.printStackTrace();
            }

        }
    };

    private boolean checkCameraPermission() {
        int permissionCheck = ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_CALENDAR);
        if(permissionCheck == PackageManager.PERMISSION_GRANTED)
        {
            return true;
        }
        else
        {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.CAMERA},
                    42);
        }
        return true;
    }

    private Size mPreviewSize = null;
    private CameraDevice mCameraDevice = null;
    private CaptureRequest.Builder mPreviewBuilder = null;
    private CameraCaptureSession mPreviewSession = null;
    private ImageReader imageReader = null;
    private CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(CameraDevice camera) {
            Log.i(TAG, "onOpened");
            mCameraDevice = camera;

            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            if (texture == null) {
                Log.e(TAG, "texture is null");
                return;
            }

            texture.setDefaultBufferSize(mPreviewSize.getWidth()/4, mPreviewSize.getHeight()/4);
            Surface surface = new Surface(texture);

            HandlerThread backgroundThread = new HandlerThread("CameraSend");
            backgroundThread.start();
            Handler backgroundHandler = new Handler(backgroundThread.getLooper());

            imageReader = ImageReader.newInstance(mPreviewSize.getWidth()/4,
                    mPreviewSize.getHeight()/4, ImageFormat.YUV_420_888, 2);
            imageReader.setOnImageAvailableListener(onImAvListen, backgroundHandler);

            try {
                mPreviewBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            } catch (CameraAccessException e){
                e.printStackTrace();
            }

            mPreviewBuilder.addTarget(surface);
            mPreviewBuilder.addTarget(imageReader.getSurface());

            try {
                mCameraDevice.createCaptureSession(Arrays.asList(surface, imageReader.getSurface()),
                        mPreviewStateCallback, null);
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            Log.e(TAG, "onError");
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            Log.e(TAG, "onDisconnected");
        }
    };

    private final ImageReader.OnImageAvailableListener onImAvListen =
            new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {
                    Image image = reader.acquireLatestImage();
                    if(image == null) {
                        return;
                    }
                    Mat yuv = new Mat(image.getHeight() + image.getHeight() / 2, image.getWidth(), CvType.CV_8UC1);
                    ByteBuffer buffer = image.getPlanes()[0].getBuffer();
                    final byte[] data = new byte[buffer.limit()];
                    buffer.get(data);
                    yuv.put(0, 0, data);

                    Bitmap bmp = Bitmap.createBitmap(image.getWidth(),
                            image.getHeight() + image.getHeight() / 2,
                            Bitmap.Config.RGB_565);
                    Utils.matToBitmap(yuv, bmp);
                    // From here we will send it to server
                    ByteArrayOutputStream stream = new ByteArrayOutputStream();
                    bmp.compress(Bitmap.CompressFormat.PNG, 100, stream);
                    byte[] byteArray = stream.toByteArray();
                    sendToServer(byteArray);
                    Log.d("Image available", String.format("Size: %1$d %1$d",
                            image.getWidth(), image.getHeight()));
                    image.close();
                }
            };

    public static void sendToServer(byte[] data) {
        URL url = null;
        HttpURLConnection conn = null;
        try {
            url = new URL("http://192.168.136.195:5000/api/post");
        } catch (MalformedURLException e) {
            Log.e("lkhlklkJ", "MalformedURLException");
        }
        try {
            conn = (HttpURLConnection) url.openConnection();
            conn.setDoOutput(true);
            conn.setChunkedStreamingMode(0);
            OutputStream out = new BufferedOutputStream(conn.getOutputStream());
            out.write(data);
        } catch (Exception e) {
            Log.e("oh noes", "there was bad times " + e.getMessage());
        } finally {
            conn.disconnect();
        }
    }

    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = (char)bytes[v >>> 4];
            hexChars[j * 2 + 1] = (char)bytes[v & 0x0F];
        }
        return new String(hexChars);
    }

    private CameraCaptureSession.StateCallback mPreviewStateCallback =
            new CameraCaptureSession.StateCallback() {

        @Override
        public void onConfigured(CameraCaptureSession session) {
            Log.i(TAG, "onConfigured");
            mPreviewSession = session;

            mPreviewBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);

            HandlerThread backgroundThread = new HandlerThread("CameraPreview");
            backgroundThread.start();
            Handler backgroundHandler = new Handler(backgroundThread.getLooper());

            try {
                mPreviewSession.setRepeatingRequest(mPreviewBuilder.build(),
                        null, backgroundHandler);
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }

        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            Log.e(TAG, "CameraCaptureSession Configure failed");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //same as set-up android:screenOrientation="portrait" in <activity>, AndroidManifest.xml
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        setContentView(R.layout.activity_camera_feed);

        Button hello = (Button) findViewById(R.id.stopRecButton);
        hello.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mPreviewSession != null) {
                    try {
                        mPreviewSession.stopRepeating();
                    }
                    catch(CameraAccessException e) {
                        Log.e("Oh shit", "We had a CameraAccessException");
                    }
                }
            }
        });

        mTextureView = (TextureView) findViewById(R.id.textureView1);
        mTextureView.setSurfaceTextureListener(mSurfaceTextureListner);
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();

        if (mCameraDevice != null)
        {
            mCameraDevice.close();
            mCameraDevice = null;
        }
    }


}
