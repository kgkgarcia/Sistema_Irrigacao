const express = require('express');
const http = require('http');
const bodyParser = require('body-parser');

const app = express();
const server = http.createServer(app);

const port = 3000;

let humidityData = []; // Dados do sensor
let waterStatus = 'off'; // Estado inicial da água

// Middleware
app.use(bodyParser.json());
app.use(express.static('public')); // Diretório para o index.html

// Rota para receber dados do sensor
app.post('/sensor', (req, res) => {
  const { humidity } = req.body;

  if (humidity !== undefined) {
    const timestamp = new Date();
    humidityData.push({ time: timestamp, humidity });

    if (humidityData.length > 100) {
      humidityData.shift(); // Remove o dado mais antigo
    }

    console.log(`Humidade recebida: ${humidity}`);
    res.status(200).send('Dados do sensor recebidos com sucesso.');
  } else {
    res.status(400).send('Humidade não fornecida.');
  }
});

// Rota para controlar o sistema de irrigação
app.post('/control', (req, res) => {
  const { action } = req.body;

  if (action === 'on' || action === 'off') {
    waterStatus = action;
    console.log(`Estado da água atualizado para: ${action}`);
    res.status(200).send(`Ação de controle recebida: ${action}`);
  } else {
    res.status(400).send('Ação inválida.');
  }
});

// Rota para enviar o estado da água
app.get('/control', (req, res) => {
  res.send(waterStatus); // Retorna o estado atual ("on" ou "off")
});

// Rota para enviar os dados do sensor
app.get('/sensor', (req, res) => {
  res.json(humidityData); // Retorna os dados do sensor como JSON
});

// Inicia o servidor
server.listen(port, () => {
  console.log(`Servidor rodando na porta ${port}`);
});
