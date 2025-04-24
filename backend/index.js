// Import required modules
const express = require('express');
const cors = require('cors');

// Initialize the app
const app = express();
const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());

// Basic route
app.get('/', (req, res) => {
	res.send('Welcome to the TinyMLpp backend!');
});             

app.post('/train', (req, res) => {
	const { algorithm, parameters } = req.body;

	// Check if algorithm is provided
	if (!algorithm) {
		return res.status(400).send('Algorithm is required.');
	}

	// Validate parameters
	if (!parameters) {
		return res.status(400).send('Parameters are required.');
	}

	// Validate dataset
	const dataset = req.body.dataset;
	if (!dataset) {
		return res.status(400).send('Dataset is required.');
	}

	// Save the dataset file
	const fs = require('fs');
	const path = require('path');
	const datasetDir = path.join(__dirname, '../datasets');
	const datasetPath = path.join(datasetDir, 'dataset.csv');

	// Ensure the datasets directory exists
	if (!fs.existsSync(datasetDir)) {
		fs.mkdirSync(datasetDir, { recursive: true });
	}

	fs.writeFile(datasetPath, dataset, (writeErr) => {
		if (writeErr) {
			console.error('Error saving dataset:', writeErr);
			return res.status(500).send('Error saving dataset.');
		}
		
		// Process the request
		const { exec } = require('child_process');
		const formattedParameters = Object.entries(parameters)
			.map(([key, value]) => `${key}=${value}`)
			.join(',');
		const command = `./demo --model "${algorithm}" --parameters "${formattedParameters},dataset=${datasetPath}"`;
		console.log(command);
		exec(command, (error, stdout, stderr) => {
			if (error) {
				console.error(`Error executing command: ${error.message}`);
				return res.status(500).send({ error: error.message });
			}
			if (stderr && error) {
				console.error(`Error output: ${stderr}`);
				return res.status(500).send({ error: stderr });
			}
			console.log(`Command output: ${stdout}`);

			// Read and process the metrics.json file
			readMetricsFile(path.join(__dirname, './metrics.json'), res, req);
		});
	});
});

// Function to read and process the metrics.json file
function readMetricsFile(metricsPath, res, req) {
	const fs = require('fs');

	fs.readFile(metricsPath, 'utf8', (readErr, data) => {
		if (readErr) {
			console.error('Error reading metrics file:', readErr);
			return res.status(500).send({ error: 'Error reading metrics file.' });
		}

		// Parse the metrics.json file
		let metrics;
		try {
			metrics = JSON.parse(data);
			console.log('Original metrics from file:', metrics);
		} catch (parseErr) {
			console.error('Error parsing metrics file:', parseErr);
			return res.status(500).send({ error: 'Error parsing metrics file.' });
		}

		// Construct the response object
		const response = {
			metrics: {},
			parameters: req.body.parameters || {},
		};

		// Copy all metrics from the file to the response exactly as they appear
		// Format numeric values as percentages if appropriate, but maintain original keys
		for (const [key, value] of Object.entries(metrics)) {
			// These fields should be kept as is (not formatted as percentages)
			const nonPercentageFields = ['time_ms', 'memory_kb', 'inertia', 'img'];
			if (nonPercentageFields.includes(key)) {
				response.metrics[key] = value;
			} else {
				// Format as percentage for metrics like accuracy, precision, recall, f1_score, r2
				response.metrics[key] = `${value.toFixed(2)}%`;
			}
		}

		console.log('Response metrics:', response.metrics);
		// Send the response as JSON
		res.json(response);
	});
}

// Endpoint to save prediction features
app.post('/save-features', (req, res) => {
	const { features } = req.body;
	if (!features) {
		return res.status(400).json({ error: 'No features provided' });
	}
	
	const fs = require('fs');
	const path = require('path');
	const featuresPath = path.join(__dirname, 'features.txt');
	
	// Save features to features.txt
	fs.writeFile(featuresPath, features, (err) => {
		if (err) {
			console.error('Error saving features:', err);
			return res.status(500).json({ error: 'Failed to save features' });
		}
		
		console.log('Features saved successfully:', features);
		res.json({ success: true, message: 'Features saved successfully' });
	});
});

// Start the server             
app.listen(PORT, () => {    
	console.log(`Server is running on http://localhost:${PORT}`);
});
