// Initialize when document is loaded
document.addEventListener("DOMContentLoaded", () => {
  console.log("TinyML dashboard initialized");

  initThemeToggle();
  initCurrentDate();
  initAlgorithmSelection();
  initTabSwitching();
  initRangeSliders();
  initCharts();
  initProgressBars();
  initAnimations();
  initAlgorithmCards();
  initFileUpload();
  initNotifications();
  initTrainingWorkflow();
  initPredictionFeatures();
});

// Theme toggle functionality
function initThemeToggle() {
  const themeToggle = document.querySelector(".theme-toggle");
  const body = document.body;
  const icon = themeToggle.querySelector("i");

  themeToggle.addEventListener("click", () => {
    body.setAttribute(
      "data-theme",
      body.getAttribute("data-theme") === "dark" ? "light" : "dark"
    );
    icon.classList.toggle("fa-moon");
    icon.classList.toggle("fa-sun");

    // Save preference to localStorage
    localStorage.setItem("theme", body.getAttribute("data-theme") || "light");
  });

  // Check for saved theme preference
  const savedTheme = localStorage.getItem("theme");
  if (savedTheme) {
    body.setAttribute("data-theme", savedTheme);
    if (savedTheme === "dark") {
      icon.classList.remove("fa-moon");
      icon.classList.add("fa-sun");
    }
  }
}

// Set current date
function initCurrentDate() {
  const dateElement = document.getElementById("current-date");
  if (dateElement) {
    const now = new Date();
    const options = { month: "long", day: "numeric", year: "numeric" };
    dateElement.textContent = now.toLocaleDateString("en-US", options);
  }
}

// Algorithm selection
function initAlgorithmSelection() {
  const algorithmCards = document.querySelectorAll(".algorithm-card");

  algorithmCards.forEach((card) => {
    card.addEventListener("click", () => {
      // Remove active class from all cards
      algorithmCards.forEach((c) => c.classList.remove("active"));

      // Add active class to clicked card
      card.classList.add("active");

      // Update parameter form based on selected algorithm
      updateParameterForm(card.getAttribute("data-algorithm"));
    });
  });
}

// Tab switching
function initTabSwitching() {
  const tabButtons = document.querySelectorAll(".tab-btn");

  tabButtons.forEach((btn) => {
    btn.addEventListener("click", () => {
      // Remove active class from all buttons
      const parentTabs = btn.closest(".parameter-tabs");
      parentTabs
        .querySelectorAll(".tab-btn")
        .forEach((b) => b.classList.remove("active"));

      // Add active class to clicked button
      btn.classList.add("active");

      // You can implement show/hide content based on tab here
    });
  });
}

// Initialize range sliders
function initRangeSliders() {
  const rangeInputs = document.querySelectorAll('input[type="range"]');

  rangeInputs.forEach((input) => {
    const valueDisplay = input.nextElementSibling;

    // Set initial value
    if (valueDisplay) {
      valueDisplay.textContent = input.value;
    }

    // Update value on change
    input.addEventListener("input", () => {
      if (valueDisplay) {
        valueDisplay.textContent = input.value;
      }
    });
  });
}

// Initialize charts
function initCharts() {
  initVisualizationChart();
}

function initVisualizationChart() {
  const ctx = document.getElementById("visualization-chart");
  if (!ctx) return;

  new Chart(ctx, {
    type: "scatter",
    data: {
      datasets: [
        {
          label: "Class A",
          data: generateRandomPoints(20, 10, 20, 10, 20),
          backgroundColor: "rgba(91, 134, 229, 0.6)",
          borderColor: "rgba(91, 134, 229, 1)",
          borderWidth: 1,
          pointRadius: 6,
          pointHoverRadius: 8,
        },
        {
          label: "Class B",
          data: generateRandomPoints(20, 30, 40, 30, 40),
          backgroundColor: "rgba(56, 178, 172, 0.6)",
          borderColor: "rgba(56, 178, 172, 1)",
          borderWidth: 1,
          pointRadius: 6,
          pointHoverRadius: 8,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      animation: {
        duration: 1000,
        easing: "easeOutQuart",
      },
      scales: {
        x: {
          beginAtZero: true,
          grid: {
            color: "rgba(0, 0, 0, 0.05)",
          },
        },
        y: {
          beginAtZero: true,
          grid: {
            color: "rgba(0, 0, 0, 0.05)",
          },
        },
      },
      plugins: {
        legend: {
          position: "top",
        },
      },
    },
  });
}

// Generate random points for scatter plot
function generateRandomPoints(count, minX, maxX, minY, maxY) {
  const points = [];
  for (let i = 0; i < count; i++) {
    points.push({
      x: Math.random() * (maxX - minX) + minX,
      y: Math.random() * (maxY - minY) + minY,
    });
  }
  return points;
}

// Initialize progress bars
function initProgressBars() {
  // Set widths for progress bars
  document.querySelectorAll(".progress-bar").forEach((bar) => {
    const width = bar.style.width || "0%";
    bar.style.width = "0%";

    // Use setTimeout to create animation effect
    setTimeout(() => {
      bar.style.width = width;
    }, 300);
  });

  // Set width for confidence fill
  const confidenceFill = document.querySelector(".confidence-fill");
  if (confidenceFill) {
    const width = confidenceFill.style.width || "0%";
    confidenceFill.style.width = "0%";

    setTimeout(() => {
      confidenceFill.style.width = width;
    }, 500);
  }
}

// Update parameter form based on selected algorithm
function updateParameterForm(algorithm) {
  const parameterForm = document.getElementById("parameter-form");
  if (!parameterForm) return;

  let formHTML = "";

  switch (algorithm) {
    case "linear-regression":
      formHTML = `
                <div class="form-group">
                    <label for="learning-rate">Learning Rate</label>
                    <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                    <span class="value-display">0.01</span>
                </div>
                <div class="form-group">
                    <label for="epochs">Epochs</label>
                    <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                    <span class="value-display">500</span>
                </div>
            `;
      break;
    case "logistic-regression":
      formHTML = `
                <div class="form-group">
                    <label for="learning-rate">Learning Rate</label>
                    <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                    <span class="value-display">0.01</span>
                </div>
                <div class="form-group">
                    <label for="epochs">Epochs</label>
                    <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                    <span class="value-display">500</span>
                </div>
            `;
      break;
    case "knn":
      formHTML = `
                <div class="form-group">
                    <label for="k">Number of Neighbors (k)</label>
                    <input type="range" id="k" min="1" max="20" step="1" value="5">
                    <span class="value-display">5</span>
                </div>
            `;
      break;
    case "k_means_clustering":
      formHTML = `
                <div class="form-group">
                    <label for="k">Number of Clusters (k)</label>
                    <input type="range" id="k" min="2" max="10" step="1" value="3">
                    <span class="value-display">3</span>
                </div>
                <div class="form-group">
                    <label for="epochs">Max Iterations</label>
                    <input type="range" id="epochs" min="100" max="1000" step="100" value="300">
                    <span class="value-display">300</span>
                </div>
            `;
      break;
    case "decision-tree":
      formHTML = `
                <div class="form-group">
                    <label for="max-depth">Max Depth</label>
                    <input type="range" id="max-depth" min="1" max="20" step="1" value="5">
                    <span class="value-display">5</span>
                </div>
                <div class="form-group">
                    <label for="min-samples-split">Min Samples Split</label>
                    <input type="range" id="min-samples-split" min="2" max="20" step="1" value="2">
                    <span class="value-display">2</span>
                </div>
            `;
      break;
    case "svm":
      formHTML = `
                <div class="form-group">
                    <label for="c">Regularization Parameter (C)</label>
                    <input type="range" id="c" min="0.1" max="10" step="0.1" value="1">
                    <span class="value-display">1</span>
                </div>
                <div class="form-group">
                    <label for="learning-rate">Learning Rate</label>
                    <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                    <span class="value-display">0.01</span>
                </div>
                <div class="form-group">
                    <label for="epochs">Epochs</label>
                    <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                    <span class="value-display">500</span>
                </div>
            `;
      break;
    default:
      formHTML = "<p>Please select an algorithm</p>";
  }

  parameterForm.innerHTML = formHTML;
  initRangeSliders();

  // Add fade-in animation
  parameterForm.style.opacity = 0;
  setTimeout(() => {
    parameterForm.style.opacity = 1;
  }, 10);
}

// Animation for the entire page
function initAnimations() {
  // Animate progress bars on scroll
  const progressBars = document.querySelectorAll(".progress-bar");
  const confidenceFill = document.querySelector(".confidence-fill");

  // Add to elements to observe list
  const elementsToObserve = [...progressBars];
  if (confidenceFill) elementsToObserve.push(confidenceFill);

  // Add stat cards to observe list
  document.querySelectorAll(".stat-card").forEach((card) => {
    elementsToObserve.push(card);
  });

  // Create intersection observer
  const observer = new IntersectionObserver(
    (entries) => {
      entries.forEach((entry) => {
        if (entry.isIntersecting) {
          if (
            entry.target.classList.contains("progress-bar") ||
            entry.target.classList.contains("confidence-fill")
          ) {
            const width = entry.target.style.width || "0%";
            entry.target.style.width = "0%";
            setTimeout(() => {
              entry.target.style.width = width;
            }, 300);
          } else if (entry.target.classList.contains("stat-card")) {
            entry.target.style.opacity = 1;
            entry.target.style.transform = "translateY(0)";
          }
          observer.unobserve(entry.target);
        }
      });
    },
    { threshold: 0.1 }
  );

  // Observe elements
  elementsToObserve.forEach((el) => {
    observer.observe(el);
  });
}

// Button click handlers
document.addEventListener("click", (e) => {
  // Train button
  if (e.target.closest("#train-btn")) {
    simulateTraining();
  }

  // Predict button
  if (e.target.closest("#predict-btn")) {
    simulatePrediction();
  }
});

// Simulate training process
function simulateTraining() {
  const progressFill = document.querySelector(".progress-fill");
  const progressPercentage = document.querySelector(".progress-percentage");
  const progressTime = document.querySelector(".progress-time");

  if (!progressFill || !progressPercentage || !progressTime) return;

  // Reset progress
  progressFill.style.width = "0%";
  progressPercentage.textContent = "0%";
  progressTime.textContent = "Estimated time: calculating...";

  // Add loading class to train button
  const trainBtn = document.getElementById("train-btn");
  if (trainBtn) {
    trainBtn.disabled = true;
    trainBtn.innerHTML = '<div class="loading-spinner"></div> Training...';
  }

  // Simulate progress
  let progress = 0;
  const interval = setInterval(() => {
    progress += Math.random() * 10;
    if (progress > 100) progress = 100;

    progressFill.style.width = `${progress}%`;
    progressPercentage.textContent = `${Math.round(progress)}%`;

    const remainingTime = Math.round((100 - progress) / 10);
    progressTime.textContent = `Estimated time: ${remainingTime}s`;

    if (progress === 100) {
      clearInterval(interval);
      progressTime.textContent = "Completed in 0.8s";

      if (trainBtn) {
        setTimeout(() => {
          trainBtn.disabled = false;
          trainBtn.innerHTML = '<i class="fas fa-play"></i> Train Model';
        }, 1000);
      }
    }
  }, 200);
}

// Modify the simulatePrediction function to send features to backend
function simulatePrediction() {
  const predictionResult = document.getElementById("prediction-result");
  const predictionValue = document.getElementById("prediction-value");

  if (!predictionResult || !predictionValue) return;

  // Add loading class to predict button
  const predictBtn = document.getElementById("predict-btn");
  if (predictBtn) {
    predictBtn.disabled = true;
    predictBtn.innerHTML = '<div class="loading-spinner"></div> Predicting...';
  }

  // Collect feature values from inputs
  const featureInputs = document.querySelectorAll(
    "#prediction-inputs .input-field input"
  );
  const features = {};
  let missingValues = false;
  const featureValues = [];

  featureInputs.forEach((input) => {
    const featureName = input.getAttribute("data-feature");
    const featureType = input.getAttribute("data-type");
    const value = input.value.trim();

    if (!value) {
      missingValues = true;
      input.classList.add("error");
      setTimeout(() => input.classList.remove("error"), 2000);
      return;
    }

    // Convert to appropriate type
    if (featureType === "numeric") {
      features[featureName] = parseFloat(value);
      featureValues.push(parseFloat(value));
    } else {
      features[featureName] = value;
      featureValues.push(value);
    }
  });

  if (missingValues) {
    if (predictBtn) {
      predictBtn.disabled = false;
      predictBtn.innerHTML = '<i class="fas fa-magic"></i> Predict';
    }
    showNotification("Please fill in all feature values", "warning");
    return;
  }

  console.log("Prediction features:", features);
  
  // Save features to backend
  predict(featureValues);

  // Show prediction result container
  predictionResult.style.display = "block";
  if (predictBtn) {
    predictBtn.disabled = false;
    predictBtn.innerHTML = '<i class="fas fa-magic"></i> Predict';
  }
}

// Add a new function to save features to backend
async function predict(featureValues) {
  try {
    const featuresString = featureValues.join(', ');
    const response = await fetch('http://localhost:3000/predict', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        features: featuresString,
        algorithm: document.querySelector(".algorithm-card.active")?.getAttribute("data-algorithm") || "default-algorithm",
        k: ["knn", "k_means_clustering"].includes(document.querySelector(".algorithm-card.active")?.getAttribute("data-algorithm"))
          ? parseInt(document.getElementById("k")?.value || 3)
          : null,
      }),
    });

    if (!response.ok) {
      throw new Error('Failed to save features');
    }

    const data = await response.json();

    // Extract prediction and display it
    if (data && data.prediction) {
        console.log("Prediction:", data.prediction.prediction);
        document.getElementById("prediction-value").textContent = data.prediction.prediction;
    } else {
        console.error("Prediction not found in response:", data);
        showNotification("Error: Prediction not found in response", "error");
    }
  } catch (error) {
    console.error('Error saving features:', error);
    showNotification('Error saving features: ' + error.message, 'error', 5000);
  }
}

function initAlgorithmCards() {
  const cards = document.querySelectorAll(".algorithm-card");
  cards.forEach((card) => {
    const icon = card.querySelector(".algorithm-icon i");
    if (!icon) return;

    // Ensure icon is properly initialized
    icon.style.display = "inline-block";
    icon.style.fontSize = "1.5rem";
    icon.style.color = "var(--primary-color)";
    icon.style.transition = "all 0.3s ease";

    card.addEventListener("mouseenter", () => {
      icon.style.transform = "scale(1.1)";
      icon.style.color = "var(--primary-hover)";
    });

    card.addEventListener("mouseleave", () => {
      icon.style.transform = "scale(1)";
      icon.style.color = "var(--primary-color)";
    });
  });
}

// Initialize the complete training workflow
function initTrainingWorkflow() {
  const algorithmCards = document.querySelectorAll(".algorithm-card");
  const parameterForm = document.getElementById("parameter-form");
  const trainButton = document.getElementById("train-button");
  const modelResults = document.getElementById("model-results");

  if (!algorithmCards.length || !parameterForm || !trainButton) {
    console.error("Required training elements not found");
    return;
  }

  let selectedAlgorithm = null;
  const trainingHistory = [];

  // Clear the history list initially
  const historyList = document.querySelector(".history-list");
  if (historyList) {
    historyList.innerHTML = '';
  }

  // Algorithm selection
  algorithmCards.forEach((card) => {
    card.addEventListener("click", () => {
      // Deselect all cards first
      algorithmCards.forEach((c) => c.classList.remove("active"));

      // Select the clicked card
      card.classList.add("active");
      selectedAlgorithm = card.getAttribute("data-algorithm");

      // Update parameter form for the selected algorithm
      updateParameterForm(selectedAlgorithm);

      // Show notification
      const algoName = card.querySelector(".algorithm-content h3").textContent;
      showNotification(`Selected ${algoName} algorithm`, "info", 2000);
    });
  });

  // Update parameter form based on selected algorithm
  function updateParameterForm(algorithm) {
    if (!algorithm) return;

    let formHTML = "";

    // Define parameters for different algorithms
    switch (algorithm) {
      case "linear-regression":
        formHTML = `
                    <div class="form-group">
                        <label for="learning-rate">Learning Rate</label>
                        <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                        <span class="value-display">0.01</span>
                    </div>
                    <div class="form-group">
                        <label for="epochs">Epochs</label>
                        <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                        <span class="value-display">500</span>
                    </div>
                `;
        break;
      case "logistic-regression":
        formHTML = `
                    <div class="form-group">
                        <label for="learning-rate">Learning Rate</label>
                        <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                        <span class="value-display">0.01</span>
                    </div>
                    <div class="form-group">
                        <label for="epochs">Epochs</label>
                        <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                        <span class="value-display">500</span>
                    </div>
                `;
        break;
      case "knn":
        formHTML = `
                    <div class="form-group">
                        <label for="k">Number of Neighbors (k)</label>
                        <input type="range" id="k" min="1" max="20" step="1" value="5">
                        <span class="value-display">5</span>
                    </div>
                `;
        break;
      case "k_means_clustering":
        formHTML = `
                    <div class="form-group">
                        <label for="k">Number of Clusters (k)</label>
                        <input type="range" id="k" min="2" max="10" step="1" value="3">
                        <span class="value-display">3</span>
                    </div>
                    <div class="form-group">
                        <label for="epochs">Max Iterations</label>
                        <input type="range" id="epochs" min="100" max="1000" step="100" value="300">
                        <span class="value-display">300</span>
                    </div>
                `;
        break;
      case "decision-tree":
        formHTML = `
                    <div class="form-group">
                        <label for="max-depth">Max Depth</label>
                        <input type="range" id="max-depth" min="1" max="20" step="1" value="5">
                        <span class="value-display">5</span>
                    </div>
                    <div class="form-group">
                        <label for="min-samples-split">Min Samples Split</label>
                        <input type="range" id="min-samples-split" min="2" max="20" step="1" value="2">
                        <span class="value-display">2</span>
                    </div>
                `;
        break;
      case "svm":
        formHTML = `
                    <div class="form-group">
                        <label for="c">Regularization Parameter (C)</label>
                        <input type="range" id="c" min="0.1" max="10" step="0.1" value="1">
                        <span class="value-display">1</span>
                    </div>
                    <div class="form-group">
                        <label for="learning-rate">Learning Rate</label>
                        <input type="range" id="learning-rate" min="0.001" max="0.1" step="0.001" value="0.01">
                        <span class="value-display">0.01</span>
                    </div>
                    <div class="form-group">
                        <label for="epochs">Epochs</label>
                        <input type="range" id="epochs" min="100" max="1000" step="100" value="500">
                        <span class="value-display">500</span>
                    </div>
                `;
        break;
      default:
        formHTML =
          '<p class="no-algo-message">Please select an algorithm first</p>';
    }

    parameterForm.innerHTML = formHTML;

    // Create animated entrance effect
    parameterForm.style.opacity = 0;
    parameterForm.style.transform = "translateY(10px)";

    setTimeout(() => {
      parameterForm.style.opacity = 1;
      parameterForm.style.transform = "translateY(0)";
    }, 10);

    // Initialize range sliders for the new form
    initFormRangeSliders(parameterForm);
  }

  // Initialize range sliders specifically for the parameter form
  function initFormRangeSliders(form) {
    const rangeInputs = form.querySelectorAll('input[type="range"]');

    rangeInputs.forEach((input) => {
      const valueDisplay = input.nextElementSibling;

      // Set initial value
      if (valueDisplay) {
        valueDisplay.textContent = input.value;
      }

      // Update value on change
      input.addEventListener("input", () => {
        if (valueDisplay) {
          valueDisplay.textContent = input.value;
        }
      });
    });
  }

  // Handle train button click
  trainButton.addEventListener("click", async (event) => {
    event.preventDefault();

    // Validate algorithm selection
    if (!selectedAlgorithm) {
      showNotification("Please select an algorithm first", "error");

      // Visual hint - scroll to and highlight algorithm section
      const algorithmSection = document.querySelector(".algorithm-selection");
      if (algorithmSection) {
        algorithmSection.scrollIntoView({ behavior: "smooth" });
        algorithmSection.classList.add("highlight-section");
        setTimeout(() => {
          algorithmSection.classList.remove("highlight-section");
        }, 2000);
      }
      return;
    }

    // Check if dataset is uploaded
    const fileInput = document.getElementById("fileInput");
    if (!fileInput || fileInput.files.length === 0) {
      showNotification("Please upload a dataset first", "error");

      // Visual hint - scroll to and highlight upload section
      const uploadSection = document.querySelector(".file-upload-section");
      if (uploadSection) {
        uploadSection.scrollIntoView({ behavior: "smooth" });
        uploadSection.classList.add("highlight-section");
        setTimeout(() => {
          uploadSection.classList.remove("highlight-section");
        }, 2000);
      }
      return;
    }

    try {
      // Disable button and show loading state
      trainButton.disabled = true;
      trainButton.innerHTML =
        '<div class="processing-spinner"></div> Training...';

      // Show training progress notification
      const trainingNotificationId = Date.now();
      const trainingNotification = showNotification(
        "Training model, please wait...",
        "info",
        0
      ); // No auto-dismiss
      trainingNotification.setAttribute("data-id", trainingNotificationId);

      // Get the uploaded dataset content
      const file = fileInput.files[0];
      const datasetContent = await new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => resolve(reader.result);
        reader.onerror = () => reject(new Error("Failed to read file"));
        reader.readAsText(file);
      });

      // Get parameters from form
      const params = {};
      parameterForm.querySelectorAll('input[type="range"]').forEach((input) => {
        const id = input.id.replace(/-/g, "_");
        params[id] = parseFloat(input.value);
      });

      // Prepare required parameters for API based on algorithm
      const apiParams = {
        lr: params.learning_rate || 0.01,
        epochs: params.epochs || 500,
      };

      // Add algorithm-specific parameters
      if (
        selectedAlgorithm === "knn" ||
        selectedAlgorithm === "k_means_clustering"
      ) {
        apiParams.k = params.k || 3;
      }

      if (selectedAlgorithm === "svm") {
        apiParams.C = params.c || 1.0;
      }

      // Convert the algorithm name to the format expected by the backend
      const backendAlgorithm = formatAlgorithmName(selectedAlgorithm);

      console.log(`Training ${backendAlgorithm} with parameters:`, apiParams);

      // Make API call to train the model
      const response = await fetch("http://localhost:3000/train", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          algorithm: backendAlgorithm,
          parameters: apiParams,
          dataset: datasetContent,
        }),
      });

      // Always close the training notification regardless of outcome
      closeNotification(trainingNotification);

      if (!response.ok) {
        const errorData = await response.text();
        throw new Error(`Training failed: ${errorData}`);
      }

      // Parse the response to get metrics
      const result = await response.json();
      const { metrics, img } = result;

      // Decode the base64 image and display it in the visualization chart
      if (img) {
        const visualizationImage = document.getElementById(
          "visualization-image"
        );
        if (visualizationImage) {
          visualizationImage.src = `data:image/png;base64,${img}`;
          visualizationImage.style.display = "block";
        } else {
          console.warn("Visualization image element not found.");
        }
        // Hide the placeholder text
        const placeholder = document.getElementById(
          "visualization-placeholder"
        );
        if (placeholder) {
          placeholder.classList.add("hidden");
        }
      }
      // Process metrics based on algorithm type

      // Decode the base64 image and display it in the visualization chart
      if (metrics.img) {
        const visualizationImage = document.getElementById(
          "visualization-image"
        );
        if (visualizationImage) {
          visualizationImage.src = `data:image/png;base64,${metrics.img}`;
          visualizationImage.style.display = "block";
        } else {
          console.warn("Visualization image element not found.");
        }
        // Hide the placeholder text
        const placeholder = document.getElementById(
          "visualization-placeholder"
        );
        if (placeholder) {
          placeholder.classList.add("hidden");
        }
      }
      const processedMetrics = processMetricsByAlgorithm(
        selectedAlgorithm,
        metrics
      );

      // Add to training history
      const trainingRecord = {
        algorithm: selectedAlgorithm,
        parameters: apiParams,
        metrics: processedMetrics,
        timestamp: new Date().toISOString(),
      };
      trainingHistory.push(trainingRecord);

      // Scroll to results section
      const resultsSection = document.querySelector(".results-section");
      if (resultsSection) {
        resultsSection.scrollIntoView({ behavior: "smooth" });
      }

      // Display results with animation
      displayMetricsByAlgorithm(
        selectedAlgorithm,
        processedMetrics,
        file,
        modelResults
      );

      // Update visualization if applicable
      updateVisualizationWithResults(selectedAlgorithm, processedMetrics);

      // Update history section
      updateTrainingHistory(trainingHistory);

      // Don't try to update performance comparison since we removed it
      // updatePerformanceComparison(processedMetrics);

      showNotification("Model trained successfully!", "success");
    } catch (error) {
      console.error("Error training model:", error);
      showNotification(error.message || "Error training model", "error");
    } finally {
      trainButton.disabled = false;
      trainButton.innerHTML = '<i class="fas fa-cogs"></i> Train Model';

      // Make sure the training notification is closed
      const existingTrainingNotifications =
        document.querySelectorAll(".notification");
      existingTrainingNotifications.forEach((notification) => {
        if (
          notification.querySelector("p").textContent ===
          "Training model, please wait..."
        ) {
          closeNotification(notification);
        }
      });
    }
  });

  // Process metrics based on algorithm type
  function processMetricsByAlgorithm(algorithm, metrics) {
    console.log("Received metrics:", metrics); // Log received metrics for debugging

    // Default processed metrics object with zeros
    const defaultMetrics = {
      accuracy: 0,
      precision: 0,
      recall: 0,
      f1_score: 0,
      r2: 0,
      inertia: 0,
      time_ms: 0,
      memory_kb: 0,
    };

    // Process string percentage values to numbers if needed
    const processValue = (value) => {
      if (typeof value === "string" && value.includes("%")) {
        return parseFloat(value.replace("%", ""));
      }
      return parseFloat(value) || 0;
    };

    // Create processed metrics object based on what's available in the response
    const processed = {
      ...defaultMetrics,
      parameters: metrics.parameters || {},
    };

    // Copy all received metrics to processed object
    for (const [key, value] of Object.entries(metrics)) {
      if (key !== "parameters") {
        processed[key] = processValue(value);
      }
    }

    return processed;
  }

  // Display metrics based on algorithm type
  function displayMetricsByAlgorithm(algorithm, metrics, file, container) {
    console.log("Displaying metrics for", algorithm, ":", metrics);

    // Common header and footer for all algorithm types
    const header = `
            <div class="result-card">
                <h3>Training Results for ${formatAlgorithmDisplayName(
                  algorithm
                )}</h3>
        `;

    const footer = `
                <div class="training-time">
                    <p>Trained on ${
                      file
                        ? `${file.name} (${formatFileSize(file.size)})`
                        : "dataset"
                    }</p>
                    <p>Training completed at ${new Date().toLocaleTimeString()}</p>
                    ${
                      metrics.time_ms
                        ? `<p>Training time: ${(metrics.time_ms / 1000).toFixed(
                            2
                          )}s</p>`
                        : ""
                    }
                    ${
                      metrics.memory_kb
                        ? `<p>Memory usage: ${formatFileSize(
                            metrics.memory_kb * 1024
                          )}</p>`
                        : ""
                    }
                </div>
            </div>
        `;

    let metricsHTML = "";

    // Different metrics display based on algorithm type and available metrics
    if (metrics.r2 && algorithm === "linear-regression") {
      // Linear Regression - R² Score and MSE
      metricsHTML = `
                <div class="metrics-grid">
                    <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.1s; opacity: 0;">
                        <span class="metric-label">R² Score</span>
                        <span class="metric-value">${metrics.r2.toFixed(
                          2
                        )}%</span>
                        <span class="metric-description">Proportion of variance explained by the model</span>
                    </div>
                    ${
                      metrics.mse
                        ? `
                    <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.2s; opacity: 0;">
                        <span class="metric-label">Mean Squared Error</span>
                        <span class="metric-value">${metrics.mse.toFixed(
                          2
                        )}</span>
                        <span class="metric-description">Average squared difference between predicted and actual values</span>
                    </div>
                    `
                        : ""
                    }
                </div>
            `;
    } else if (metrics.inertia && algorithm === "k_means_clustering") {
      // K-means Clustering - Inertia
      metricsHTML = `
                <div class="metrics-grid single-metric">
                    <div class="metric large-metric" style="animation: fadeIn 0.3s ease forwards; opacity: 0;">
                        <span class="metric-label">Inertia</span>
                        <span class="metric-value">${metrics.inertia.toFixed(
                          2
                        )}</span>
                        <span class="metric-description">Sum of squared distances to the nearest centroid</span>
                    </div>
                </div>
            `;
    } else if (
      metrics.accuracy ||
      metrics.precision ||
      metrics.recall ||
      metrics.f1_score
    ) {
      // Classification metrics - Show whichever are available
      metricsHTML = `
                <div class="metrics-grid">
                    ${
                      metrics.accuracy
                        ? `
                        <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.1s; opacity: 0;">
                            <span class="metric-label">Accuracy</span>
                            <span class="metric-value">${metrics.accuracy.toFixed(
                              2
                            )}%</span>
                        </div>
                    `
                        : ""
                    }
                    ${
                      metrics.precision
                        ? `
                        <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.2s; opacity: 0;">
                            <span class="metric-label">Precision</span>
                            <span class="metric-value">${metrics.precision.toFixed(
                              2
                            )}%</span>
                        </div>
                    `
                        : ""
                    }
                    ${
                      metrics.recall
                        ? `
                        <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.3s; opacity: 0;">
                            <span class="metric-label">Recall</span>
                            <span class="metric-value">${metrics.recall.toFixed(
                              2
                            )}%</span>
                        </div>
                    `
                        : ""
                    }
                    ${
                      metrics.f1_score
                        ? `
                        <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: 0.4s; opacity: 0;">
                            <span class="metric-label">F1 Score</span>
                            <span class="metric-value">${metrics.f1_score.toFixed(
                              2
                            )}%</span>
                        </div>
                    `
                        : ""
                    }
                </div>
            `;
    } else {
      // For any other metrics, display them dynamically
      const metricsToShow = Object.entries(metrics).filter(
        ([key]) => !["parameters", "time_ms", "memory_kb"].includes(key)
      );

      if (metricsToShow.length > 0) {
        metricsHTML = `
                    <div class="metrics-grid">
                        ${metricsToShow
                          .map(
                            ([key, value], index) => `
                            <div class="metric" style="animation: fadeIn 0.3s ease forwards; animation-delay: ${
                              0.1 * index
                            }s; opacity: 0;">
                                <span class="metric-label">${formatMetricName(
                                  key
                                )}</span>
                                <span class="metric-value">${
                                  typeof value === "number"
                                    ? value.toFixed(2)
                                    : value
                                }${key !== "inertia" ? "%" : ""}</span>
                            </div>
                        `
                          )
                          .join("")}
                    </div>
                `;
      } else {
        metricsHTML = `
                    <div class="metrics-grid">
                        <div class="metric" style="animation: fadeIn 0.3s ease forwards; opacity: 0;">
                            <span class="metric-label">No Metrics Available</span>
                            <span class="metric-value">-</span>
                        </div>
                    </div>
                `;
      }
    }

    // Set the full HTML
    container.innerHTML = header + metricsHTML + footer;

    // Update prediction features
    updatePredictionFeatures(algorithm, metrics, currentDatasetInfo);
  }

  // Format algorithm name for display
  function formatAlgorithmDisplayName(algorithm) {
    if (algorithm === "k_means_clustering") {
      return "K-Means Clustering";
    }
    return algorithm
      .split("-")
      .map((word) => word.charAt(0).toUpperCase() + word.slice(1))
      .join(" ");
  }

  // Format algorithm name for backend API
  function formatAlgorithmName(algorithm) {
    if (algorithm === "k-means") {
      return "k_means_clustering";
    }
    return algorithm.replace(/-/g, "_");
  }

  // Format parameter name for display
  function formatParameterName(name) {
    switch (name) {
      case "lr":
        return "Learning Rate";
      case "epochs":
        return "Epochs";
      case "k":
        return "K Value";
      case "C":
        return "Regularization (C)";
      default:
        return name
          .split("_")
          .map((word) => word.charAt(0).toUpperCase() + word.slice(1))
          .join(" ");
    }
  }

  // Format metric name for display
  function formatMetricName(name) {
    switch (name) {
      case "r2":
        return "R² Score";
      case "f1_score":
        return "F1 Score";
      case "inertia":
        return "Inertia";
      default:
        return name
          .split("_")
          .map((word) => word.charAt(0).toUpperCase() + word.slice(1))
          .join(" ");
    }
  }

  // Update training history display
  function updateTrainingHistory(history) {
    const historyList = document.querySelector(".history-list");
    if (!historyList) return;

    historyList.innerHTML = history
      .map((record, index) => {
        // Determine which metric to show in the history item
        let metricDisplay = "";
        if (record.algorithm === "linear-regression" && record.metrics.r2) {
          metricDisplay = `R² Score: ${record.metrics.r2.toFixed(2)}%`;
        } else if (
          record.algorithm === "k_means_clustering" &&
          record.metrics.inertia
        ) {
          metricDisplay = `Inertia: ${record.metrics.inertia.toFixed(2)}`;
        } else if (record.metrics.accuracy) {
          metricDisplay = `Accuracy: ${record.metrics.accuracy.toFixed(2)}%`;
        } else if (record.metrics.f1_score) {
          metricDisplay = `F1 Score: ${record.metrics.f1_score.toFixed(2)}%`;
        } else {
          // Find first available metric
          const metricKey = Object.keys(record.metrics).find(
            (key) => !["parameters", "time_ms", "memory_kb"].includes(key)
          );
          if (metricKey) {
            const metricValue = record.metrics[metricKey];
            metricDisplay = `${formatMetricName(metricKey)}: ${
              typeof metricValue === "number"
                ? metricValue.toFixed(2)
                : metricValue
            }${metricKey !== "inertia" ? "%" : ""}`;
          } else {
            metricDisplay = "No metrics available";
          }
        }

        return `
                <div class="history-item" style="animation: fadeIn 0.3s ease forwards; animation-delay: ${
                  0.1 * index
                }s; opacity: 0;">
                    <div class="history-icon ${record.algorithm}">
                        <i class="fas fa-${getAlgorithmIcon(
                          record.algorithm
                        )}"></i>
                    </div>
                    <div class="history-details">
                        <div class="history-name">${formatAlgorithmDisplayName(
                          record.algorithm
                        )} #${index + 1}</div>
                        <div class="history-meta">
                            <span class="history-time">${new Date(
                              record.timestamp
                            ).toLocaleString()}</span>
                            <span class="history-accuracy">${metricDisplay}</span>
                        </div>
                    </div>
                    <div class="history-actions">
                        <button class="action-btn" onclick="showModelDetails(${index})">
                            <i class="fas fa-eye"></i>
                        </button>
                    </div>
                </div>
            `;
      })
      .join("");
  }

  // Update visualization with training results
  function updateVisualizationWithResults(algorithm, metrics) {
    // This would be expanded in a real implementation to show actual visualizations
    // based on the algorithm type and training results
    console.log(
      `Visualization would be updated for ${algorithm} with metrics:`,
      metrics
    );
  }

  // Get appropriate icon for algorithm
  function getAlgorithmIcon(algorithm) {
    const icons = {
      "linear-regression": "chart-line",
      "logistic-regression": "wave-square",
      knn: "users",
      k_means_clustering: "object-group",
      "decision-tree": "project-diagram",
      svm: "border-all",
    };
    return icons[algorithm] || "cogs";
  }

  // Make this function available globally for the history item buttons
  window.showModelDetails = function (index) {
    if (index >= trainingHistory.length) return;

    const record = trainingHistory[index];

    // Display metrics based on algorithm type
    displayMetricsByAlgorithm(
      record.algorithm,
      record.metrics,
      null,
      modelResults
    );

    // Scroll to results
    modelResults.scrollIntoView({ behavior: "smooth" });
  };
}

// Notification system
function initNotifications() {
  // Create notification container if it doesn't exist
  if (!document.querySelector(".notification-container")) {
    const container = document.createElement("div");
    container.className = "notification-container";
    document.body.appendChild(container);
  }
}

// Show notification function
function showNotification(message, type = "info", duration = 5000) {
  // Create container if it doesn't exist
  if (!document.querySelector(".notification-container")) {
    const container = document.createElement("div");
    container.className = "notification-container";
    document.body.appendChild(container);
  }

  const container = document.querySelector(".notification-container");

  // Remove any existing notifications with the same message
  const existingNotifications = container.querySelectorAll(".notification");
  existingNotifications.forEach((notification) => {
    if (notification.querySelector("p").textContent === message) {
      notification.classList.remove("show");
      setTimeout(() => {
        notification.remove();
      }, 300);
    }
  });

  const notification = document.createElement("div");
  notification.className = `notification notification-${type}`;
  notification.setAttribute("data-id", Date.now()); // Add unique ID to reference later

  // Add icon based on type
  const icon =
    type === "success"
      ? "check-circle"
      : type === "error"
      ? "exclamation-circle"
      : type === "warning"
      ? "exclamation-triangle"
      : "info-circle";

  notification.innerHTML = `
        <div class="notification-icon">
            <i class="fas fa-${icon}"></i>
        </div>
        <div class="notification-content">
            <p>${message}</p>
        </div>
        <button class="notification-close">
            <i class="fas fa-times"></i>
        </button>
    `;

  // Add to container
  container.appendChild(notification);

  // Add animation
  setTimeout(() => {
    notification.classList.add("show");
  }, 10);

  // Set up close button
  const closeBtn = notification.querySelector(".notification-close");
  closeBtn.addEventListener("click", () => {
    closeNotification(notification);
  });

  // Auto-remove after duration if not 0
  if (duration > 0) {
    setTimeout(() => {
      closeNotification(notification);
    }, duration);
  }

  // Return the notification for reference
  return notification;
}

// Close notification helper function
function closeNotification(notification) {
  if (!notification || !notification.parentNode) return;

  notification.classList.remove("show");
  setTimeout(() => {
    if (notification.parentNode) {
      notification.remove();
    }
  }, 300);
}

// File upload functionality
function initFileUpload() {
  const fileInput = document.getElementById("fileInput");
  const fileList = document.getElementById("fileList");
  const dropZone = document.getElementById("dropZone");

  if (!fileInput || !fileList || !dropZone) return;

  // Display uploaded files with improved styling
  fileInput.addEventListener("change", async () => {
    if (fileInput.files.length === 0) return;

    fileList.innerHTML = "";
    const file = fileInput.files[0]; // Only use the first file for now

    try {
      // Create file item with additional info
      const fileItem = document.createElement("div");
      fileItem.className = "file-item";

      // File type icon based on extension
      const extension = file.name.split(".").pop().toLowerCase();
      const iconClass =
        extension === "csv"
          ? "fa-file-csv"
          : extension === "json"
          ? "fa-file-code"
          : extension === "xlsx"
          ? "fa-file-excel"
          : "fa-file";

      fileItem.innerHTML = `
                <div class="file-icon">
                    <i class="fas ${iconClass}"></i>
                </div>
                <div class="file-info">
                    <div class="file-name">${file.name}</div>
                    <div class="file-size">${formatFileSize(file.size)}</div>
                </div>
                <button class="remove-file">
                    <i class="fas fa-times"></i>
                </button>
                <div class="file-processing">
                    <div class="processing-indicator">
                        <div class="processing-spinner"></div>
                        <span>Analyzing dataset...</span>
                    </div>
                </div>
            `;

      fileList.appendChild(fileItem);

      // Remove file button
      const removeBtn = fileItem.querySelector(".remove-file");
      removeBtn.addEventListener("click", () => {
        fileInput.value = "";
        fileItem.classList.add("removing");
        setTimeout(() => {
          fileList.innerHTML = "";
          document.getElementById("dataset-info").innerHTML = "";
        }, 300);
      });

      // Show success notification
      showNotification(`File "${file.name}" uploaded successfully`, "success");

      // Process and display dataset info
      await processDataset(file);

      // Remove processing indicator
      const processingIndicator = fileItem.querySelector(".file-processing");
      processingIndicator.remove();
    } catch (error) {
      console.error("Error processing file:", error);
      showNotification("Error processing dataset: " + error.message, "error");
    }
  });

  // Drag and drop functionality
  dropZone.addEventListener("dragover", (e) => {
    e.preventDefault();
    dropZone.classList.add("drag-over");
  });

  dropZone.addEventListener("dragleave", () => {
    dropZone.classList.remove("drag-over");
  });

  dropZone.addEventListener("drop", (e) => {
    e.preventDefault();
    dropZone.classList.remove("drag-over");
    fileInput.files = e.dataTransfer.files;
    fileInput.dispatchEvent(new Event("change"));
  });
}

// Format file size
function formatFileSize(bytes) {
  if (bytes === 0) return "0 Bytes";
  const k = 1024;
  const sizes = ["Bytes", "KB", "MB", "GB"];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
}

// Process dataset and show preview
async function processDataset(file) {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();

    reader.onload = (e) => {
      try {
        const content = e.target.result;
        const extension = file.name.split(".").pop().toLowerCase();

        // Process based on file type
        if (extension === "csv") {
          const data = parseCSV(content);
          displayDatasetInfo(data, file.name);
          resolve(data);
        } else if (extension === "json") {
          const data = JSON.parse(content);
          displayDatasetInfo(data, file.name);
          resolve(data);
        } else {
          throw new Error("Unsupported file format");
        }
      } catch (error) {
        console.error("Error parsing file:", error);
        reject(error);
      }
    };

    reader.onerror = () => {
      reject(new Error("Failed to read file"));
    };

    reader.readAsText(file);
  });
}

// Parse CSV content
function parseCSV(content) {
  const lines = content.split("\n");
  if (lines.length === 0) throw new Error("Empty file");

  // Parse header
  const header = lines[0].split(",").map((h) => h.trim());

  // Parse data rows
  const data = [];
  for (let i = 1; i < lines.length; i++) {
    if (lines[i].trim() === "") continue;

    const values = lines[i].split(",").map((v) => v.trim());
    if (values.length !== header.length) {
      console.warn(
        `Line ${i} has ${values.length} values instead of ${header.length}`
      );
      continue;
    }

    const row = {};
    header.forEach((col, j) => {
      row[col] = values[j];
    });
    data.push(row);
  }

  // Detect column types
  const columnTypes = {};
  header.forEach((column) => {
    const columnValues = data.map((row) => row[column]);
    const numericValues = columnValues.filter(
      (val) => !isNaN(parseFloat(val)) && isFinite(val)
    );
    columnTypes[column] =
      numericValues.length > 0.7 * columnValues.length
        ? "numeric"
        : "categorical";
  });

  currentDatasetInfo = {
    header,
    data,
    rowCount: data.length,
    columnCount: header.length,
    columnTypes,
  };

  return currentDatasetInfo;
}

// Display dataset information
function displayDatasetInfo(data, filename) {
  // Create dataset info container if it doesn't exist
  let infoContainer = document.getElementById("dataset-info");
  if (!infoContainer) {
    infoContainer = document.createElement("div");
    infoContainer.id = "dataset-info";
    infoContainer.className = "dataset-info";

    // Find the right place to insert it
    const fileList = document.getElementById("fileList");
    if (fileList) {
      fileList.parentNode.insertBefore(infoContainer, fileList.nextSibling);
    }
  }

  // Calculate some basic stats
  const stats = analyzeDataset(data);

  // Build the HTML
  infoContainer.innerHTML = `
        <div class="dataset-summary">
            <h3>Dataset Summary: ${filename}</h3>
            <div class="summary-metrics">
                <div class="metric">
                    <i class="fas fa-table"></i>
                    <span>${data.rowCount}</span>
                    <label>Rows</label>
                </div>
                <div class="metric">
                    <i class="fas fa-columns"></i>
                    <span>${data.columnCount}</span>
                    <label>Columns</label>
                </div>
                <div class="metric">
                    <i class="fas fa-star"></i>
                    <span>${stats.numericColumns.length}</span>
                    <label>Numeric Features</label>
                </div>
                <div class="metric">
                    <i class="fas fa-tag"></i>
                    <span>${stats.categoricalColumns.length}</span>
                    <label>Categorical Features</label>
                </div>
            </div>
        </div>
        
        <div class="dataset-preview">
            <h4>Data Preview</h4>
            <div class="preview-table-container">
                <table class="preview-table">
                    <thead>
                        <tr>
                            ${data.header
                              .map((col) => `<th>${col}</th>`)
                              .join("")}
                        </tr>
                    </thead>
                    <tbody>
                        ${data.data
                          .slice(0, 5)
                          .map(
                            (row) => `
                            <tr>
                                ${data.header
                                  .map((col) => `<td>${row[col]}</td>`)
                                  .join("")}
                            </tr>
                        `
                          )
                          .join("")}
                    </tbody>
                </table>
            </div>
            <div class="preview-fade"></div>
        </div>
        
        <div class="column-stats">
            <h4>Column Statistics</h4>
            <div class="column-stats-grid">
                ${stats.columnStats
                  .slice(0, 4)
                  .map(
                    (col) => `
                    <div class="column-stat-card">
                        <div class="column-name">${
                          col.name
                        } <span class="column-type">${col.type}</span></div>
                        <div class="stat-values">
                            ${
                              col.type === "numeric"
                                ? `
                                <div class="stat-item">
                                    <span>Min: </span><span>${col.min.toFixed(
                                      2
                                    )}</span>
                                </div>
                                <div class="stat-item">
                                    <span>Max: </span><span>${col.max.toFixed(
                                      2
                                    )}</span>
                                </div>
                                <div class="stat-item">
                                    <span>Mean: </span><span>${col.mean.toFixed(
                                      2
                                    )}</span>
                                </div>
                            `
                                : `
                                <div class="stat-item">
                                    <span>Unique Values: </span><span>${col.uniqueValues.length}</span>
                                </div>
                                <div class="stat-item">
                                    <span>Most Common: </span><span>${col.mostCommon.value} (${col.mostCommon.count})</span>
                                </div>
                            `
                            }
                        </div>
                    </div>
                `
                  )
                  .join("")}
            </div>
        </div>
    `;

  // Add animation
  infoContainer.style.opacity = 0;
  setTimeout(() => {
    infoContainer.style.opacity = 1;
  }, 10);
}

// Analyze dataset columns
function analyzeDataset(data) {
  const numericColumns = [];
  const categoricalColumns = [];
  const columnStats = [];

  // Analyze each column
  data.header.forEach((col) => {
    const values = data.data.map((row) => row[col]);

    // Check if column is numeric
    const numericValues = values
      .map((v) => parseFloat(v))
      .filter((v) => !isNaN(v));
    const isNumeric = numericValues.length > 0.5 * values.length;

    if (isNumeric) {
      numericColumns.push(col);

      // Calculate stats
      const min = Math.min(...numericValues);
      const max = Math.max(...numericValues);
      const sum = numericValues.reduce((a, b) => a + b, 0);
      const mean = sum / numericValues.length;

      columnStats.push({
        name: col,
        type: "numeric",
        min,
        max,
        mean,
        uniqueValues: [...new Set(numericValues)],
        nullCount: values.filter(
          (v) => v === "" || v === null || v === undefined
        ).length,
      });
    } else {
      categoricalColumns.push(col);

      // Calculate stats
      const uniqueValues = [
        ...new Set(
          values.filter((v) => v !== "" && v !== null && v !== undefined)
        ),
      ];

      // Count frequency of each value
      const valueCounts = {};
      values.forEach((v) => {
        if (v !== "" && v !== null && v !== undefined) {
          valueCounts[v] = (valueCounts[v] || 0) + 1;
        }
      });

      // Find most common value
      let mostCommon = { value: "", count: 0 };
      Object.entries(valueCounts).forEach(([value, count]) => {
        if (count > mostCommon.count) {
          mostCommon = { value, count };
        }
      });

      columnStats.push({
        name: col,
        type: "categorical",
        uniqueValues,
        valueCounts,
        mostCommon,
        nullCount: values.filter(
          (v) => v === "" || v === null || v === undefined
        ).length,
      });
    }
  });

  return {
    numericColumns,
    categoricalColumns,
    columnStats,
  };
}

// Global variable to store current dataset information
let currentDatasetInfo = null;
let modelTrained = false;

// Initialize prediction features
function initPredictionFeatures() {
  const predictButton = document.getElementById("predict-btn");
  const predictionInputs = document.getElementById("prediction-inputs");
  const noModelMessage = document.getElementById("no-model-message");

  if (!predictButton || !predictionInputs || !noModelMessage) return;

  // Initially disable predict button
  predictButton.disabled = true;

  // Handle predict button click
  predictButton.addEventListener("click", (e) => {
    e.preventDefault();

    if (!modelTrained) {
      showNotification("Please train a model first", "warning");
      return;
    }

    simulatePrediction();
  });
}

// Update prediction UI after model training
function updatePredictionFeatures(algorithm, metrics, datasetInfo) {
  // Show prediction inputs and enable prediction button
  const noModelMessage = document.getElementById("no-model-message");
  const predictionInputs = document.getElementById("prediction-inputs");
  const predictButton = document.getElementById("predict-btn");
  const predictionResult = document.getElementById("prediction-result");

  if (
    !noModelMessage ||
    !predictionInputs ||
    !predictButton ||
    !predictionResult
  )
    return;

  // Reset prediction result
  const predictionValue = document.getElementById("prediction-value");
  if (predictionValue) {
    predictionValue.textContent = "--";
  }

  // If we have dataset information, create input fields
  if (datasetInfo && datasetInfo.header) {
    // Clear existing inputs
    predictionInputs.innerHTML = "";

    // Analyze column types if not already done
    const columnTypes = {};
    if (!datasetInfo.columnTypes) {
      datasetInfo.header.forEach((feature) => {
        // Simple heuristic: check first 5 values
        const values = datasetInfo.data.slice(0, 5).map((row) => row[feature]);
        const numericValues = values.filter(
          (val) => !isNaN(parseFloat(val)) && isFinite(val)
        );
        columnTypes[feature] =
          numericValues.length >= values.length / 2 ? "numeric" : "categorical";
      });
      datasetInfo.columnTypes = columnTypes;
    }

    // Create input fields for each feature
    datasetInfo.header.forEach((feature, index) => {
      // Skip target variable (usually last column) for supervised algorithms
      if (
        index === datasetInfo.header.length - 1 &&
        algorithm !== "k_means_clustering"
      )
        return;

      const type = datasetInfo.columnTypes[feature] || "categorical";

      let inputHTML = "";
      if (type === "numeric") {
        inputHTML = `
                    <div class="input-field" style="animation: fadeIn 0.3s ease forwards; animation-delay: ${
                      0.1 * index
                    }s; opacity: 0;">
                        <label>${feature}</label>
                        <input type="number" step="any" data-feature="${feature}" data-type="numeric" placeholder="Enter numeric value">
                    </div>
                `;
      } else {
        inputHTML = `
                    <div class="input-field" style="animation: fadeIn 0.3s ease forwards; animation-delay: ${
                      0.1 * index
                    }s; opacity: 0;">
                        <label>${feature}</label>
                        <input type="text" data-feature="${feature}" data-type="categorical" placeholder="Enter categorical value">
                    </div>
                `;
      }

      predictionInputs.innerHTML += inputHTML;
    });

    // Show inputs and enable predict button
    noModelMessage.style.display = "none";
    predictionInputs.style.display = "flex";
    predictButton.disabled = false;
    modelTrained = true;
  }
}
