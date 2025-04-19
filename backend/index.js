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
		const command = `./demo --model "${algorithm}" --parameters "${formattedParameters}" --dataset "${datasetPath}"`;
		console.log(command);
		exec(command, (error, stdout, stderr) => {
			if (error) {
				console.error(`Error executing command: ${error.message}`);
				return res.status(500).send(`Error: ${error.message}`);
			}
			if (stderr) {
				console.error(`Error output: ${stderr}`);
				return res.status(500).send(`Error: ${stderr}`);
			}

			// Send the command output as the response
			res.status(200).send({
				message: 'Training completed successfully.',
				output: stdout.trim()
			});
		});
	});
});

// Start the server             
app.listen(PORT, () => {    
	console.log(`Server is running on http://localhost:${PORT}`);
});
