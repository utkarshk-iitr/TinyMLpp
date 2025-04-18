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
    const visualizationContainer = document.getElementById('visualization-chart');
    if (!visualizationContainer) return;

    // Set initial placeholder text
    visualizationContainer.innerHTML = '<p style="text-align: center; color: gray;">Visualization will appear here</p>';

}
// Function to update the chart with an image
function updateVisualizationWithImage(imageSrc) {
    visualizationContainer.innerHTML = `<img src="${imageSrc}" alt="Visualization" style="max-width: 100%; height: auto;">`;
}

// Example usage: Call this function with an image source when needed
// updateVisualizationWithImage('path/to/your/image.png');

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
    
    let html = '';
    
    switch(algorithm) {
        case 'linear-regression':
            html = `
                <div class="parameter-group">
                    <label>Learning Rate</label>
                    <div class="parameter-input">
                        <input type="range" min="0.001" max="1" step="0.001" value="0.01" id="learning-rate">
                        <span class="parameter-value">0.01</span>
                    </div>
                </div>
                <div class="parameter-group">
                    <label>Epochs</label>
                    <div class="parameter-input">
                        <input type="range" min="10" max="1000" step="10" value="100" id="epochs">
                        <span class="parameter-value">100</span>
                    </div>
                </div>
            `;
            break;
        case 'logistic-regression':
            html = `
                <div class="parameter-group">
                    <label>Learning Rate</label>
                    <div class="parameter-input">
                        <input type="range" min="0.001" max="1" step="0.001" value="0.01" id="learning-rate">
                        <span class="parameter-value">0.01</span>
                    </div>
                </div>
                <div class="parameter-group">
                    <label>Epochs</label>
                    <div class="parameter-input">
                        <input type="range" min="10" max="1000" step="10" value="100" id="epochs">
                        <span class="parameter-value">100</span>
                    </div>
                </div>
            `;
            break;
        case 'knn':
            html = `
                <div class="parameter-group">
                    <label>Number of Neighbors (k)</label>
                    <div class="parameter-input">
                        <input type="range" min="1" max="20" value="5" id="k-value">
                        <span class="parameter-value">5</span>
                    </div>
                </div>
            `;
            break;
        case 'k-means':
            html = `
                <div class="parameter-group">
                    <label>Number of Clusters (k)</label>
                    <div class="parameter-input">
                        <input type="range" min="2" max="10" value="3" id="k-value">
                        <span class="parameter-value">3</span>
                    </div>
                </div>
                <div class="parameter-group">
                    <label>Max Iterations</label>
                    <div class="parameter-input">
                        <input type="range" min="10" max="500" step="10" value="100" id="max-iterations">
                        <span class="parameter-value">100</span>
                    </div>
                </div>
            `;
            break;
        case 'decision-tree':
            html = `
                <div class="parameter-group">
                    <label>Max Depth</label>
                    <div class="parameter-input">
                        <input type="range" min="1" max="20" value="5" id="depth-value">
                        <span class="parameter-value">5</span>
                    </div>
                </div>
                <div class="parameter-group">
                    <label>Min Samples Split</label>
                    <div class="parameter-input">
                        <input type="range" min="2" max="20" value="2" id="min-samples-split">
                        <span class="parameter-value">2</span>
                    </div>
                </div>
            `;
            break;
        case 'svm':
            html = `
                <div class="parameter-group">
                    <label>Regularization Parameter (C)</label>
                    <div class="parameter-input">
                        <input type="range" min="0.1" max="10" step="0.1" value="1" id="c-value">
                        <span class="parameter-value">1</span>
                    </div>
                </div>
            `;
            break;
        default:
            html = '<p>Please select an algorithm</p>';
    }
    
    parameterForm.innerHTML = html;
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
    const modelHistory = document.getElementById('model-history');

    if (!algorithmCards.length || !parameterForm || !trainButton || !modelResults || !modelHistory) {
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
                    <div class="form-group">
                        <label for="regularization">Regularization</label>
                        <input type="range" id="regularization" min="0" max="1" step="0.1" value="0.1">
                        <span class="value-display">0.1</span>
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
                    <div class="form-group">
                        <label for="regularization">Regularization</label>
                        <input type="range" id="regularization" min="0" max="1" step="0.1" value="0.1">
                        <span class="value-display">0.1</span>
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
                    <div class="form-group">
                        <label for="weights">Weights</label>
                        <select id="weights">
                            <option value="uniform">Uniform</option>
                            <option value="distance">Distance</option>
                        </select>
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
                    <div class="form-group">
                        <label for="init">Initialization Method</label>
                        <select id="init">
                            <option value="k-means++">k-means++</option>
                            <option value="random">Random</option>
                        </select>
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
                    <div class="form-group">
                        <label for="criterion">Criterion</label>
                        <select id="criterion">
                            <option value="gini">Gini</option>
                            <option value="entropy">Entropy</option>
                        </select>
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
                    <div class="form-group">
                        <label for="kernel">Kernel</label>
                        <select id="kernel">
                            <option value="linear">Linear</option>
                            <option value="rbf">RBF</option>
                            <option value="poly">Polynomial</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label for="gamma">Gamma</label>
                        <input type="range" id="gamma" min="0.001" max="1" step="0.001" value="0.1">
                        <span class="value-display">0.1</span>
                    </div>
                `;
                break;
        }

        parameterForm.innerHTML = formHTML;

        // Add event listeners for value displays
        parameterForm.querySelectorAll('input[type="range"], select').forEach(input => {
            const display = input.nextElementSibling;
            input.addEventListener('input', () => {
                display.textContent = input.value;
            });
        });
    };

    // Handle model training
    trainButton.addEventListener('click', async () => {
        if (!currentAlgorithm) {
            showNotification('Please select an algorithm first', 'error');
            return;
        }

        try {
            trainButton.disabled = true;
            trainButton.textContent = 'Training...';

            // Get parameters
            const params = {};
            parameterForm.querySelectorAll('input[type="range"], select').forEach(input => {
                params[input.id] = input.type === 'range' ? parseFloat(input.value) : input.value;
            });
            console.log("parameters", {params});
            // Send parameters and model name to the backend
            const response = await fetch('http://localhost:3000/train', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    algorithm: currentAlgorithm,
                    parameters: params,
                }),
            });

            if (!response.ok) {
                throw new Error('Failed to train model on the backend');
            }

            // Get metrics from the backend response
            const responseData = await response.json();
            const { metrics, image } = responseData;

            // Update visualization window with the image
            if (image) {
                updateVisualizationWithImage(`data:image/png;base64,${image}`);
            }

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
        modelHistory.innerHTML = trainingHistory.map((record, index) => `
            <div class="history-item">
                <div class="history-header">
                    <span class="history-algorithm">${record.algorithm}</span>
                    <span class="history-timestamp">${new Date(record.timestamp).toLocaleString()}</span>
                </div>
                <div class="history-metrics">
                    <span class="metric">Accuracy: ${record.metrics.accuracy}%</span>
                    <span class="metric">F1: ${record.metrics.f1}%</span>
                </div>
                <button class="view-details" data-index="${index}">View Details</button>
            </div>
        `).join('');

        // Add event listeners to view details buttons
        modelHistory.querySelectorAll('.view-details').forEach(button => {
            button.addEventListener('click', () => {
                const index = button.dataset.index;
                const record = trainingHistory[index];
                showTrainingDetails(record);
            });
        });
    };

    // Show detailed training results
    const showTrainingDetails = (record) => {
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