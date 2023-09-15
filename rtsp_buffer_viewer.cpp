#include <opencv2/opencv.hpp>
#include <vector>
#include <thread>



// Function to add a frame to the buffer, ensuring it doesn't exceed the specified size
void addToBuffer(std::vector<cv::Mat>& buffer, const cv::Mat& frame, int buffer_size) {
    if (buffer.size() >= buffer_size) {
        // Remove the oldest frame
        buffer.erase(buffer.begin());
    }
    buffer.push_back(frame);
}


// Function to generate a timestamped video filename
std::string generateVideoFilename() {
    std::time_t now = std::time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    std::ostringstream oss;
    oss << "video_" << std::put_time(&timeinfo, "%Y%m%d-%H%M%S") << ".mp4";
    return oss.str();
}


int main() {
    // Define rtsp camera
    const std::string rtsp_url = "rtsp://192.168.0.100/cam0_0";
    cv::VideoCapture cap(rtsp_url);
    bool camera_opened = false;

    // Define frame buffers to store frames for 2.5 seconds each
    const int buffer_size = 25 * 2.5; // Assuming 25 FPS
    std::vector<cv::Mat> frame_buffer_before;
    std::vector<cv::Mat> frame_buffer_after;

    bool buffer_switched = false;

    while (!camera_opened) {
        cap.open(rtsp_url);
        if (cap.isOpened()) {
            camera_opened = true;
        } else {
            std::cerr << "Error: Could not open the RTSP camera stream. Retrying..." << std::endl;
        }
    }

    cv::namedWindow("RTSP Camera Feed", cv::WINDOW_NORMAL);

    while (true) {
        cv::Mat frame;

        if (!cap.read(frame)) {
            std::cerr << "Warning: Failed to read frame from the RTSP camera. Retrying..." << std::endl;
            cap.release();
            cap.open(rtsp_url);
            cv::waitKey(1000);
            continue;
        }

        // Add the frame to the current buffer
        if (!buffer_switched) {
            addToBuffer(frame_buffer_before, frame, buffer_size);
        } 
        else {
            addToBuffer(frame_buffer_after, frame, buffer_size);
        }

        // Display the frame in the window
        cv::imshow("RTSP Camera Feed", frame);

        // Check for user input to exit or start recording
        char key = cv::waitKey(10);

        if (key == 27) {  // Press ESC key to exit
            break;
        } 
        else if (key == 32) {  // Press space key to start recording
            buffer_switched = true;
        }


        // Check if both buffers are full
        if (frame_buffer_before.size() >= buffer_size && frame_buffer_after.size() >= buffer_size) {
            // Concatenate frame_buffer_before and frame_buffer_after
            std::vector<cv::Mat> concatenated_frames(frame_buffer_before.begin(), frame_buffer_before.end());
            concatenated_frames.insert(concatenated_frames.end(), frame_buffer_after.begin(), frame_buffer_after.end());

            // Generate a timestamped 5-second video with both buffers
            std::string video_filename = generateVideoFilename();

            // Define video codec and compression parameters
            int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');  // Use avc1 codec, which is H.264
            cv::VideoWriter writer(video_filename, fourcc, 25, frame.size());

            // Write frames to the video
            for (const auto& frame : concatenated_frames) {
                writer.write(frame);
            }

            // Copy frame_buffer_after to frame_buffer_before, clear and reset the buffers
            frame_buffer_before.clear();
            frame_buffer_before = frame_buffer_after;
            frame_buffer_after.clear();
            buffer_switched = false;

            std::cout << "Video saved as: " << video_filename << std::endl;
        }

    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}

