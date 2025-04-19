// Initialize when document is loaded
document.addEventListener('DOMContentLoaded', () => {
    initThemeToggle();
    initCurrentDate();
    initAlgorithmSelection();
    initTabSwitching();
    initRangeSliders();
    initCharts();
    initProgressBars();
    initAnimations();
    initAlgorithmCards();
    initModelManager();
});

// Theme toggle functionality
function initThemeToggle() {
    const themeToggle = document.querySelector('.theme-toggle');
    const body = document.body;
    const icon = themeToggle.querySelector('i');
    
    themeToggle.addEventListener('click', () => {
        body.setAttribute('data-theme', body.getAttribute('data-theme') === 'dark' ? 'light' : 'dark');
        icon.classList.toggle('fa-moon');
        icon.classList.toggle('fa-sun');
        
        // Save preference to localStorage
        localStorage.setItem('theme', body.getAttribute('data-theme') || 'light');
    });
    
    // Check for saved theme preference
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme) {
        body.setAttribute('data-theme', savedTheme);
        if (savedTheme === 'dark') {
            icon.classList.remove('fa-moon');
            icon.classList.add('fa-sun');
        }
    }
}

// Set current date
function initCurrentDate() {
    const dateElement = document.getElementById('current-date');
    if (dateElement) {
        const now = new Date();
        const options = { month: 'long', day: 'numeric', year: 'numeric' };
        dateElement.textContent = now.toLocaleDateString('en-US', options);
    }
}

// Algorithm selection
function initAlgorithmSelection() {
    const algorithmCards = document.querySelectorAll('.algorithm-card');
    
    algorithmCards.forEach(card => {
        card.addEventListener('click', () => {
            // Remove active class from all cards
            algorithmCards.forEach(c => c.classList.remove('active'));
            
            // Add active class to clicked card
            card.classList.add('active');
            
            // Update parameter form based on selected algorithm
            updateParameterForm(card.getAttribute('data-algorithm'));
        });
    });
}

// Tab switching
function initTabSwitching() {
    const tabButtons = document.querySelectorAll('.tab-btn');
    
    tabButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            // Remove active class from all buttons
            const parentTabs = btn.closest('.parameter-tabs');
            parentTabs.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            
            // Add active class to clicked button
            btn.classList.add('active');
            
            // You can implement show/hide content based on tab here
        });
    });
}

// Initialize range sliders
function initRangeSliders() {
    const rangeInputs = document.querySelectorAll('input[type="range"]');
    
    rangeInputs.forEach(input => {
        const valueDisplay = input.nextElementSibling;
        
        // Set initial value
        if (valueDisplay) {
            valueDisplay.textContent = input.value;
        }
        
        // Update value on change
        input.addEventListener('input', () => {
            if (valueDisplay) {
                valueDisplay.textContent = input.value;
            }
        });
    });
}

// Initialize charts
function initCharts() {
    initVisualizationChart();
    initComparisonChart();
}

function initVisualizationChart() {
    const ctx = document.getElementById('visualization-chart');
    if (!ctx) return;
    
    new Chart(ctx, {
        type: 'scatter',
        data: {
            datasets: [{
                label: 'Class A',
                data: generateRandomPoints(20, 10, 20, 10, 20),
                backgroundColor: 'rgba(91, 134, 229, 0.6)',
                borderColor: 'rgba(91, 134, 229, 1)',
                borderWidth: 1,
                pointRadius: 6,
                pointHoverRadius: 8
            }, {
                label: 'Class B',
                data: generateRandomPoints(20, 30, 40, 30, 40),
                backgroundColor: 'rgba(56, 178, 172, 0.6)',
                borderColor: 'rgba(56, 178, 172, 1)',
                borderWidth: 1,
                pointRadius: 6,
                pointHoverRadius: 8
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: {
                duration: 1000,
                easing: 'easeOutQuart'
            },
            scales: {
                x: {
                    beginAtZero: true,
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)'
                    }
                },
                y: {
                    beginAtZero: true,
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)'
                    }
                }
            },
            plugins: {
                legend: {
                    position: 'top',
                }
            }
        }
    });
}

function initComparisonChart() {
    const ctx = document.getElementById('comparison-chart');
    if (!ctx) return;
    
    new Chart(ctx, {
        type: 'bar',
        data: {
            labels: ['Training Time', 'Memory Usage', 'Accuracy'],
            datasets: [{
                label: 'C++',
                data: [0.8, 2.5, 95],
                backgroundColor: 'rgba(91, 134, 229, 0.6)',
                borderColor: 'rgba(91, 134, 229, 1)',
                borderWidth: 1
            }, {
                label: 'Python',
                data: [2.3, 4.6, 93],
                backgroundColor: 'rgba(246, 173, 85, 0.6)',
                borderColor: 'rgba(246, 173, 85, 1)',
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: {
                duration: 1000,
                easing: 'easeOutQuart'
            },
            scales: {
                y: {
                    beginAtZero: true,
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)'
                    }
                },
                x: {
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)'
                    }
                }
            },
            plugins: {
                legend: {
                    position: 'top',
                }
            }
        }
    });
}

// Generate random points for scatter plot
function generateRandomPoints(count, minX, maxX, minY, maxY) {
    const points = [];
    for (let i = 0; i < count; i++) {
        points.push({
            x: Math.random() * (maxX - minX) + minX,
            y: Math.random() * (maxY - minY) + minY
        });
    }
    return points;
}

// Initialize progress bars
function initProgressBars() {
    // Set widths for progress bars
    document.querySelectorAll('.progress-bar').forEach(bar => {
        const width = bar.style.width || '0%';
        bar.style.width = '0%';
        
        // Use setTimeout to create animation effect
        setTimeout(() => {
            bar.style.width = width;
        }, 300);
    });
    
    // Set width for confidence fill
    const confidenceFill = document.querySelector('.confidence-fill');
    if (confidenceFill) {
        const width = confidenceFill.style.width || '0%';
        confidenceFill.style.width = '0%';
        
        setTimeout(() => {
            confidenceFill.style.width = width;
        }, 500);
    }
}

// Update parameter form based on selected algorithm
function updateParameterForm(algorithm) {
    const parameterForm = document.getElementById('parameter-form');
    if (!parameterForm) return;
    
    let formHTML = '';
    
    switch(algorithm) {
        case 'linear-regression':
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
        case 'logistic-regression':
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
        case 'knn':
            formHTML = `
                <div class="form-group">
                    <label for="k">Number of Neighbors (k)</label>
                    <input type="range" id="k" min="1" max="20" step="1" value="5">
                    <span class="value-display">5</span>
                </div>
            `;
            break;
        case 'k-means':
            formHTML = `
                <div class="form-group">
                    <label for="k">Number of Clusters (k)</label>
                    <input type="range" id="k" min="2" max="10" step="1" value="3">
                    <span class="value-display">3</span>
                </div>
                <div class="form-group">
                    <label for="max-iterations">Max Iterations</label>
                    <input type="range" id="max-iterations" min="100" max="1000" step="100" value="300">
                    <span class="value-display">300</span>
                </div>
            `;
            break;
        case 'decision-tree':
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
        case 'svm':
            formHTML = `
                <div class="form-group">
                    <label for="c">Regularization Parameter (C)</label>
                    <input type="range" id="c" min="0.1" max="10" step="0.1" value="1">
                    <span class="value-display">1</span>
                </div>
            `;
            break;
        default:
            formHTML = '<p>Please select an algorithm</p>';
    }
    
    parameterForm.innerHTML = formHTML;
    initRangeSliders();
    
    // Add fade-in animation
    parameterForm.style.opacity = 0;
    setTimeout(() => {
        parameterForm.style.opacity = 1;
    }, 10);
}

// Plot type switching
document.addEventListener('click', (e) => {
    if (e.target.closest('.plot-btn')) {
        const btn = e.target.closest('.plot-btn');
        const plotType = btn.getAttribute('data-plot');
        
        // Remove active class from all plot buttons
        document.querySelectorAll('.plot-btn').forEach(b => b.classList.remove('active'));
        
        // Add active class to clicked button
        btn.classList.add('active');
        
        // Change plot type (in a real app, you would update the chart)
        updatePlotType(plotType);
    }
});

// Update plot type (simplified version)
function updatePlotType(type) {
    const chart = Chart.getChart('visualization-chart');
    if (!chart) return;
    
    // In a real app, you would have more complex chart updating logic
    chart.config.type = type === 'bar' ? 'bar' : type === 'line' ? 'line' : 'scatter';
    chart.update();
}

// Animation for the entire page
function initAnimations() {
    // Animate progress bars on scroll
    const progressBars = document.querySelectorAll('.progress-bar');
    const confidenceFill = document.querySelector('.confidence-fill');
    
    // Add to elements to observe list
    const elementsToObserve = [...progressBars];
    if (confidenceFill) elementsToObserve.push(confidenceFill);
    
    // Add stat cards to observe list
    document.querySelectorAll('.stat-card').forEach(card => {
        elementsToObserve.push(card);
    });
    
    // Create intersection observer
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                if (entry.target.classList.contains('progress-bar') || 
                    entry.target.classList.contains('confidence-fill')) {
                    const width = entry.target.style.width || '0%';
                    entry.target.style.width = '0%';
                    setTimeout(() => {
                        entry.target.style.width = width;
                    }, 300);
                } else if (entry.target.classList.contains('stat-card')) {
                    entry.target.style.opacity = 1;
                    entry.target.style.transform = 'translateY(0)';
                }
                observer.unobserve(entry.target);
            }
        });
    }, { threshold: 0.1 });
    
    // Observe elements
    elementsToObserve.forEach(el => {
        observer.observe(el);
    });
}

// Button click handlers
document.addEventListener('click', (e) => {
    // Train button
    if (e.target.closest('#train-btn')) {
        simulateTraining();
    }
    
    // Predict button
    if (e.target.closest('#predict-btn')) {
        simulatePrediction();
    }
});

// Simulate training process
function simulateTraining() {
    const progressFill = document.querySelector('.progress-fill');
    const progressPercentage = document.querySelector('.progress-percentage');
    const progressTime = document.querySelector('.progress-time');
    
    if (!progressFill || !progressPercentage || !progressTime) return;
    
    // Reset progress
    progressFill.style.width = '0%';
    progressPercentage.textContent = '0%';
    progressTime.textContent = 'Estimated time: calculating...';
    
    // Add loading class to train button
    const trainBtn = document.getElementById('train-btn');
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
            progressTime.textContent = 'Completed in 0.8s';
            
            if (trainBtn) {
                setTimeout(() => {
                    trainBtn.disabled = false;
                    trainBtn.innerHTML = '<i class="fas fa-play"></i> Train Model';
                }, 1000);
            }
        }
    }, 200);
}

// Simulate prediction
function simulatePrediction() {
    const predictionResult = document.getElementById('prediction-result');
    const confidenceFill = document.querySelector('.confidence-fill');
    const confidenceValue = document.querySelector('.confidence-value');
    
    if (!predictionResult || !confidenceFill || !confidenceValue) return;
    
    // Add loading class to predict button
    const predictBtn = document.getElementById('predict-btn');
    if (predictBtn) {
        predictBtn.disabled = true;
        predictBtn.innerHTML = '<div class="loading-spinner"></div> Predicting...';
    }
    
    // Reset confidence
    confidenceFill.style.width = '0%';
    
    // Simulate loading
    setTimeout(() => {
        // Update result
        document.querySelector('.result-value').textContent = 'Class A';
        
        // Animate confidence bar
        setTimeout(() => {
            confidenceFill.style.width = '85%';
            confidenceValue.textContent = '85%';
        }, 300);
        
        if (predictBtn) {
            predictBtn.disabled = false;
            predictBtn.innerHTML = '<i class="fas fa-magic"></i> Predict';
        }
    }, 1500);
}

function initAlgorithmCards() {
    const cards = document.querySelectorAll('.algorithm-card');
    cards.forEach(card => {
        const icon = card.querySelector('.algorithm-icon i');
        if (!icon) return;

        // Ensure icon is properly initialized
        icon.style.display = 'inline-block';
        icon.style.fontSize = '1.5rem';
        icon.style.color = 'var(--primary-color)';
        icon.style.transition = 'all 0.3s ease';

        card.addEventListener('mouseenter', () => {
            icon.style.transform = 'scale(1.1)';
            icon.style.color = 'var(--primary-hover)';
        });

        card.addEventListener('mouseleave', () => {
            icon.style.transform = 'scale(1)';
            icon.style.color = 'var(--primary-color)';
        });
    });
}

// Model Manager
const initModelManager = () => {
    const algorithmCards = document.querySelectorAll('.algorithm-card');
    const parameterForm = document.getElementById('parameter-form');
    const trainButton = document.getElementById('train-button');
    const modelResults = document.getElementById('model-results');
    const historyList = document.querySelector('.history-list');

    if (!algorithmCards.length || !parameterForm || !trainButton || !modelResults || !historyList) {
        console.error('Required elements not found');
        return;
    }

    let currentAlgorithm = null;
    let currentModel = null;
    let trainingHistory = [];

    // Initialize algorithm selection
    algorithmCards.forEach(card => {
        card.addEventListener('click', () => {
            algorithmCards.forEach(c => c.classList.remove('active'));
            card.classList.add('active');
            currentAlgorithm = card.dataset.algorithm;
            updateParameterForm(currentAlgorithm);
        });
    });

    // Update parameter form based on selected algorithm
    const updateParameterForm = (algorithm) => {
        let formHTML = '';

        switch (algorithm) {
            case 'linear-regression':
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
            case 'logistic-regression':
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
            case 'knn':
                formHTML = `
                    <div class="form-group">
                        <label for="k">Number of Neighbors (k)</label>
                        <input type="range" id="k" min="1" max="20" step="1" value="5">
                        <span class="value-display">5</span>
                    </div>
                `;
                break;
            case 'k-means':
                formHTML = `
                    <div class="form-group">
                        <label for="k">Number of Clusters (k)</label>
                        <input type="range" id="k" min="2" max="10" step="1" value="3">
                        <span class="value-display">3</span>
                    </div>
                    <div class="form-group">
                        <label for="max-iterations">Max Iterations</label>
                        <input type="range" id="max-iterations" min="100" max="1000" step="100" value="300">
                        <span class="value-display">300</span>
                    </div>
                `;
                break;
            case 'decision-tree':
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
            case 'svm':
                formHTML = `
                    <div class="form-group">
                        <label for="c">Regularization Parameter (C)</label>
                        <input type="range" id="c" min="0.1" max="10" step="0.1" value="1">
                        <span class="value-display">1</span>
                    </div>
                `;
                break;
        }

        parameterForm.innerHTML = formHTML;

        // Add event listeners for value displays
        parameterForm.querySelectorAll('input[type="range"]').forEach(input => {
            const display = input.nextElementSibling;
            input.addEventListener('input', () => {
                display.textContent = input.value;
            });
        });
    };

    // Handle model training
    trainButton.addEventListener('click', async (event) => {
        event.preventDefault(); // Prevent form submission and page refresh
        // Helper function to read the uploaded dataset
        const getUploadedDataset = async () => {
            const fileInput = document.getElementById('fileInput');
            if (!fileInput || fileInput.files.length === 0) {
                console.error('No dataset file uploaded');
                return Promise.reject('No dataset file uploaded');
            }
            const file = fileInput.files[0];
            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                reader.onload = () => {
                    console.log('File successfully read:', file.name);
                    resolve(reader.result);
                };
                reader.onerror = () => {
                    console.error('Failed to read the file:', file.name);
                    reject(new Error('Failed to read the file'));
                };
                reader.readAsText(file);
            });
        };
        if (!currentAlgorithm) {
            showNotification('Please select an algorithm first', 'error');
            return;
        }

        try {
            trainButton.disabled = true;

            // Handle file upload
            const fileInput = document.getElementById('fileInput');
            const fileList = document.getElementById('fileList');
            const dropZone = document.getElementById('dropZone');

            // Display uploaded files
            fileInput.addEventListener('change', () => {
                fileList.innerHTML = '';
                Array.from(fileInput.files).forEach(file => {
                    const listItem = document.createElement('div');
                    listItem.textContent = file.name;
                    fileList.appendChild(listItem);
                });
            });

            // Drag and drop functionality
            dropZone.addEventListener('dragover', (e) => {
                e.preventDefault();
                dropZone.classList.add('drag-over');
            });

            dropZone.addEventListener('dragleave', () => {
                dropZone.classList.remove('drag-over');
            });

            dropZone.addEventListener('drop', (e) => {
                e.preventDefault();
                dropZone.classList.remove('drag-over');
                fileInput.files = e.dataTransfer.files;
                fileInput.dispatchEvent(new Event('change'));
            });

            // Send file during training
            const getUploadedFile = () => {
                if (!fileInput.files.length) {
                    throw new Error('No dataset file uploaded');
                }
                return fileInput.files[0];
            };

            // Include file in training request
            const datasetFile = getUploadedFile();
            const params = {};
            parameterForm.querySelectorAll('input[type="range"]').forEach(input => {
                params[input.id] = input.type === 'range' ? parseFloat(input.value) : input.value;
            });

            // Fetch the uploaded dataset content
            const datasetContent = await getUploadedDataset();
            // Make API call to train the model
            const requiredParams = {
                lr: params['learning-rate'] !== undefined ? params['learning-rate'] : 0.01,
                epochs: params['epochs'] !== undefined ? params['epochs'] : 500,
                k: params['k'] !== undefined ? params['k'] : 5,
                C: params['c'] !== undefined ? params['c'] : 1
            };  
            if (!currentAlgorithm || !requiredParams || !datasetContent) {
                console.error('Missing required data for training:', {
                    currentAlgorithm,
                    requiredParams,
                    datasetContent
                });
                showNotification('Training failed: Missing required data', 'error');
                return;
            }
            console.log('Training with parameters:', requiredParams);
            const response = await fetch('http://localhost:3000/train', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    algorithm: currentAlgorithm,
                    parameters: requiredParams,
                    dataset: datasetContent
                })
            });

            if (!response.ok) {
                const errorDetails = await response.json();
                console.error('Training failed:', errorDetails);
                showNotification(`Training failed: ${errorDetails.message || 'Unknown error'}`, 'error');
                return;
            }

            trainButton.textContent = 'Training...';

            if (!response.ok) {
                throw new Error('Failed to train the model');
            }

            // Parse the response to get metrics
            const { metrics } = await response.json();

            // Add to training history
            const trainingRecord = {
                algorithm: currentAlgorithm,
                parameters: params,
                metrics: metrics,
                timestamp: new Date().toISOString()
            };
            trainingHistory.push(trainingRecord);

            // Display results
            modelResults.innerHTML = `
                <div class="result-card">
                    <h3>Training Results</h3>
                    <div class="metrics-grid">
                        <div class="metric">
                            <span class="metric-label">Accuracy</span>
                            <span class="metric-value">${metrics.accuracy}%</span>
                        </div>
                        <div class="metric">
                            <span class="metric-label">Precision</span>
                            <span class="metric-value">${metrics.precision}%</span>
                        </div>
                        <div class="metric">
                            <span class="metric-label">Recall</span>
                            <span class="metric-value">${metrics.recall}%</span>
                        </div>
                        <div class="metric">
                            <span class="metric-label">F1 Score</span>
                            <span class="metric-value">${metrics.f1}%</span>
                        </div>
                    </div>
                    <div class="parameters">
                        <h4>Parameters</h4>
                        <pre>${JSON.stringify(params, null, 2)}</pre>
                    </div>
                </div>
            `;

            // Update history
            updateTrainingHistory();

            showNotification('Model trained successfully', 'success');
        } catch (error) {
            console.error('Error training model:', error);
            showNotification('Error training model', 'error');
        } finally {
            trainButton.disabled = false;
            trainButton.textContent = 'Train Model';
        }
    });

    // Update training history display
    const updateTrainingHistory = () => {
        historyList.innerHTML = trainingHistory.map((record, index) => `
            <div class="history-item">
                <div class="history-icon ${record.algorithm}">
                    <i class="fas fa-${getAlgorithmIcon(record.algorithm)}"></i>
                </div>
                <div class="history-details">
                    <div class="history-name">${formatAlgorithmName(record.algorithm)} Model #${index + 1}</div>
                    <div class="history-meta">
                        <span class="history-time">${new Date(record.timestamp).toLocaleString()}</span>
                        <span class="history-accuracy">Accuracy: ${record.metrics.accuracy}%</span>
                    </div>
                </div>
                <div class="history-actions">
                    <button class="action-btn" onclick="showTrainingDetails(${index})">
                        <i class="fas fa-play"></i>
                    </button>
                    <button class="action-btn">
                        <i class="fas fa-download"></i>
                    </button>
                </div>
            </div>
        `).join('');
    };

    // Helper function to get algorithm icon
    const getAlgorithmIcon = (algorithm) => {
        const icons = {
            'linear-regression': 'chart-line',
            'logistic-regression': 'wave-square',
            'knn': 'users',
            'k-means': 'object-group',
            'decision-tree': 'project-diagram',
            'svm': 'border-all'
        };
        return icons[algorithm] || 'cogs';
    };

    // Helper function to format algorithm name
    const formatAlgorithmName = (algorithm) => {
        return algorithm.split('-')
            .map(word => word.charAt(0).toUpperCase() + word.slice(1))
            .join(' ');
    };

    // Show detailed training results
    const showTrainingDetails = (index) => {
        const record = trainingHistory[index];
        modelResults.innerHTML = `
            <div class="result-card">
                <h3>Training Details</h3>
                <div class="metrics-grid">
                    <div class="metric">
                        <span class="metric-label">Accuracy</span>
                        <span class="metric-value">${record.metrics.accuracy}%</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">Precision</span>
                        <span class="metric-value">${record.metrics.precision}%</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">Recall</span>
                        <span class="metric-value">${record.metrics.recall}%</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">F1 Score</span>
                        <span class="metric-value">${record.metrics.f1}%</span>
                    </div>
                </div>
                <div class="parameters">
                    <h4>Parameters</h4>
                    <pre>${JSON.stringify(record.parameters, null, 2)}</pre>
                </div>
                <div class="timestamp">
                    <span>Training completed: ${new Date(record.timestamp).toLocaleString()}</span>
                </div>
            </div>
        `;
    };
}; 