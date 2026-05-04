package com.example.facerecognition

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import org.opencv.android.OpenCVLoader
import org.opencv.android.Utils
import org.opencv.core.Mat
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {

    private lateinit var cameraView: PreviewView
    private lateinit var tvResult: TextView
    private lateinit var btnTrain: Button
    private lateinit var cameraExecutor: ExecutorService

    private var isTraining = false
    private val trainingFaces = mutableListOf<Mat>()
    private val trainingLabels = mutableListOf<String>()
    private var personName = "Persona1"
    private var openCVInitialized = false
    private var pcaTrained = false
    private var lastProcessTime = 0L

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        cameraView = findViewById(R.id.camera_view)
        tvResult = findViewById(R.id.tv_result)
        btnTrain = findViewById(R.id.btn_train)
        cameraExecutor = Executors.newSingleThreadExecutor()

        btnTrain.setOnClickListener {
            if (!isTraining) {
                isTraining = true
                trainingFaces.clear()
                trainingLabels.clear()
                pcaTrained = false
                btnTrain.text = "Capturando... (0/20)"
            } else {
                if (trainingFaces.size > 0) finishTraining()
            }
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
            == PackageManager.PERMISSION_GRANTED) {
            initApp()
        } else {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.CAMERA), 1)
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<String>, grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 1 && grantResults.isNotEmpty() &&
            grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            initApp()
        }
    }

    private fun initApp() {
        if (OpenCVLoader.initLocal()) {
            Log.i("OpenCV", "OpenCV cargado exitosamente")
            System.loadLibrary("facerecognition")
            val cascadePath = copyAssetToCache("haarcascade_frontalface_default.xml")
            initDetector(cascadePath)
            openCVInitialized = true
            startCamera()
        } else {
            Log.e("OpenCV", "Error al inicializar OpenCV")
        }
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()

            val preview = Preview.Builder().build().also {
                it.setSurfaceProvider(cameraView.surfaceProvider)
            }

            val imageAnalyzer = ImageAnalysis.Builder()
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build()
                .also { analysis ->
                    analysis.setAnalyzer(cameraExecutor) { imageProxy ->
                        processFrame(imageProxy)
                    }
                }

            val cameraSelector = CameraSelector.DEFAULT_FRONT_CAMERA

            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageAnalyzer)
                Log.i("Camera", "Camara iniciada correctamente")
            } catch (e: Exception) {
                Log.e("Camera", "Error al iniciar camara: ${e.message}")
            }
        }, ContextCompat.getMainExecutor(this))
    }


    private fun processFrame(imageProxy: ImageProxy) {
        if (!openCVInitialized) {
            imageProxy.close()
            return
        }

        val now = System.currentTimeMillis()
        if (now - lastProcessTime < 1000) {
            imageProxy.close()
            return
        }
        lastProcessTime = now

        val bitmap = imageProxy.toBitmap()
        imageProxy.close()

        val mat = Mat()
        Utils.bitmapToMat(bitmap, mat)

        if (isTraining && trainingFaces.size < 20) {
            trainingFaces.add(mat.clone())
            trainingLabels.add(personName)
            val count = trainingFaces.size
            runOnUiThread {
                btnTrain.text = "Capturando... ($count/20)"
            }
            if (trainingFaces.size >= 20) {
                runOnUiThread { finishTraining() }
            }
        } else if (pcaTrained) {
            val result = recognizeFace(mat.nativeObjAddr)
            runOnUiThread { tvResult.text = result }
        }

        mat.release()
    }

    private fun finishTraining() {
        isTraining = false
        if (trainingFaces.isEmpty()) {
            tvResult.text = "No hay fotos para entrenar"
            btnTrain.text = "Entrenar"
            return
        }
        val addrs = LongArray(trainingFaces.size) { trainingFaces[it].nativeObjAddr }
        val labels = Array(trainingLabels.size) { trainingLabels[it] }
        trainPCA(addrs, labels)
        pcaTrained = true
        btnTrain.text = "Entrenar nueva persona"
        tvResult.text = "Entrenado! Apunta la camara"
    }

    private fun copyAssetToCache(filename: String): String {
        val outFile = java.io.File(cacheDir, filename)
        if (!outFile.exists()) {
            assets.open(filename).use { input ->
                outFile.outputStream().use { output -> input.copyTo(output) }
            }
        }
        return outFile.absolutePath
    }

    override fun onDestroy() {
        super.onDestroy()
        cameraExecutor.shutdown()
    }

    external fun initDetector(cascadePath: String)
    external fun trainPCA(matAddrs: LongArray, labels: Array<String>)
    external fun recognizeFace(matAddr: Long): String
}