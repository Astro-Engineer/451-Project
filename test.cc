#include <iostream>
#include <vector>
#include <omp.h>
#include <time.h>

#include <fstream>
#include <sstream>
#include <string>

// Linear regression function to estimate coefficients (slope and intercept)
void linearRegression(const std::vector<double>& X, const std::vector<double>& y, double& slope, double& intercept, double learning_rate) {
    int n = X.size();
    double slope_gradient = 0.0;
    double intercept_gradient = 0.0;

    // Iterate through the data in mini-batches
    #pragma omp parallel for reduction(+:slope_gradient,intercept_gradient)
    for (int i = 0; i < n; i++) {
        double prediction = slope * X[i] + intercept;
        double error = prediction - y[i];
        slope_gradient += 2 * X[i] * error;
        intercept_gradient += 2 * error;
    }

    // Update the model parameters
    slope -= learning_rate * slope_gradient / n;
    intercept -= learning_rate * intercept_gradient / n;
}

int main() {
    // Sample data
    std::vector<double> X = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0};
    std::vector<double> y = {2.0, 4.0, 5.5, 4.8, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0};

    const std::string csvFileName = "your_file.csv";
    const std::string csvFilePath = "./" + csvFileName;
    std::ifstream inputFile(csvFilePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the CSV file." << std::endl;
        return 1; // Return an error code
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        double x, y;

        if (iss >> x >> std::ws >> std::ws >> y) {
            // Add values to the vectors
            X.push_back(x);
            y.push_back(y);
        } else {
            std::cerr << "Error: Invalid line format in the CSV file." << std::endl;
        }
    }
	
    double slope = 0.0;
    double intercept = 0.0;

    double learning_rate = 0.01;
    int num_epochs = 100;
    
    struct timespec start, stop; 
	 double exe_time;
	 srand(time(NULL)); 

    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        linearRegression(X, y, slope, intercept, learning_rate);
    }
    
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
	 exe_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	 
	 printf("Execution time = %f sec\n",  exe_time);

    // Display the results
    std::cout << "Linear Regression Results:" << std::endl;
    std::cout << "Slope: " << slope << std::endl;
    std::cout << "Intercept: " << intercept << std::endl;

    return 0;
}
