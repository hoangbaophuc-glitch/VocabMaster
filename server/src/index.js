import express from 'express';
import cors from 'cors';
import dotenv from 'dotenv';
import { playTurn, getMiniGameWord, getHint } from './controllers/gameController.js';

dotenv.config();

const app = express();
const PORT = process.env.PORT || 5000;

app.use(cors());
app.use(express.json());

// Routes
app.post('/api/game/play', playTurn);
app.post('/api/game/minigame', getMiniGameWord);
app.post('/api/game/hint', getHint);

app.get('/api/health', (req, res) => {
  res.json({ status: 'ok', message: 'Server is running C++ Engine' });
});

app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
