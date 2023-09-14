#include <opencv2/opencv.hpp>

int main() {
    // Replace this with your RTSP camera URL
    const std::string rtsp_url = "rtsp://192.168.0.100/cam0_0";

    // Create an OpenCV VideoCapture object to open the RTSP stream
    cv::VideoCapture cap(rtsp_url);

    // Check if the camera opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the RTSP camera stream." << std::endl;
        return -1;
    }

    // Create a window to display the video feed
    cv::namedWindow("RTSP Camera Feed", cv::WINDOW_NORMAL);

    while (true) {
        cv::Mat frame;

        // Read a frame from the camera
        if (!cap.read(frame)) {
            std::cerr << "Warning: Failed to read frame from the RTSP camera. Retrying..." << std::endl;
            cap.release();
            cap.open(rtsp_url);

            // Sleep for a moment before trying to reopen the stream
            cv::waitKey(1000);

            continue;
        }

        // Display the frame in the window
        cv::imshow("RTSP Camera Feed", frame);

        // Check for user input to exit the loop
        char key = cv::waitKey(10);
        if (key == 27)  // Press ESC key to exit
            break;
    }

    // Release the camera and destroy the window
    cap.release();
    cv::destroyAllWindows();

    return 0;
}

