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

app.get('/train', (req, res) => {
	const { algorithm, parameters } = req.query;

	// Check if algorithm is provided
	if (!algorithm) {
		return res.status(400).send('Algorithm is required.');
	}

	// Validate parameters
	if (!parameters || typeof parameters !== 'object') {
		return res.status(400).send('Valid parameters are required.');
	}

	// Process the request
	const { exec } = require('child_process');
	const command = `./demo --model "${algorithm}" --parameters "${parameters}"`;

	exec(command, (error, stdout, stderr) => {
		if (error) {
			console.error(`Error executing command: ${error.message}`);
			return res.status(500).send(`Error: ${error.message}`);
		}
		if (stderr) {
			console.error(`Error output: ${stderr}`);
			return res.status(500).send(`Error: ${stderr}`);
		}
		const fs = require('fs');
		const filePath = './visual.jpg'; // Replace with the actual file path created by the binary

		// Check if the file exists
		fs.access(filePath, fs.constants.F_OK, (err) => {
			if (err) {
				console.error('File not found:', filePath);
				return res.status(500).send('Output file not found.');
			}

			// Read the file and send it as base64 along with the stdout
			fs.readFile(filePath, { encoding: 'base64' }, (readErr, data) => {
				if (readErr) {
					console.error('Error reading file:', readErr);
					return res.status(500).send('Error reading output file.');
				}

				res.setHeader('Content-Type', 'application/json');
				res.send({
					message: stdout,
					file: data // Base64 encoded image
				});
			});
		});
	});
});

// Start the server             
app.listen(PORT, () => {    
	console.log(`Server is running on http://localhost:${PORT}`);
});
