import { execFile } from 'child_process';
import path from 'path';
import fs from 'fs';
import { fileURLToPath } from 'url';
import https from 'https';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Tự động nhận diện đuôi file nhị phân C++ (.exe trên Windows, không có đuôi trên Linux)
const isWindows = process.platform === 'win32';
const cppExecutableName = isWindows ? 'VocabMaster.exe' : 'VocabMaster';
const cppPath = path.resolve(__dirname, '../../cpp', cppExecutableName);
const dictPath = path.resolve(__dirname, '../../cpp/dictionary.json');

// Helper chạy tiến trình C++ và trả về Promise chứa kết quả JSON
const runCpp = (args) => {
  return new Promise((resolve, reject) => {
    execFile(cppPath, args, (error, stdout, stderr) => {
      if (error) {
        return reject(new Error(`Lỗi thực thi C++: ${error.message}. Stderr: ${stderr}`));
      }
      try {
        const parsed = JSON.parse(stdout.trim());
        resolve(parsed);
      } catch (err) {
        reject(new Error(`Lỗi parse kết quả JSON từ C++: ${stdout.trim()}. Stderr: ${stderr}`));
      }
    });
  });
};

// Hàm gọi API Từ điển Quốc tế để kiểm tra từ vựng (Fallback khi từ chưa có trong Trie của C++)
const checkWordExternally = (word) => {
  return new Promise((resolve) => {
    https.get(`https://api.dictionaryapi.dev/api/v2/entries/en/${word}`, (res) => {
      if (res.statusCode === 200) {
        let data = '';
        res.on('data', chunk => data += chunk);
        res.on('end', () => {
          try {
            const parsed = JSON.parse(data);
            const def = parsed[0]?.meanings[0]?.definitions[0]?.definition || "Từ tiếng Anh hợp lệ.";
            resolve(def);
          } catch(e) { resolve(null); }
        });
      } else {
        resolve(null);
      }
    }).on('error', () => resolve(null));
  });
};

// ĐIỀU PHỐI LƯỢT CHƠI CHÍNH
export const playTurn = async (req, res) => {
  try {
    const { word, usedWords, botLastChar, timeTaken, difficulty = 'easy' } = req.body;

    if (!word) {
      return res.status(400).json({ error: 'Vui lòng nhập từ.' });
    }

    const cleanWord = word.trim().toLowerCase();

    // 1. Chạy C++ Engine để kiểm tra từ và AI phản hồi
    let result;
    try {
      result = await runCpp([
        '--play',
        cleanWord,
        (usedWords || []).join(','),
        botLastChar || '',
        difficulty,
        '15', // timeLimit
        String(timeTaken || 0)
      ]);
    } catch (err) {
      return res.status(500).json({ error: err.message });
    }

    // 2. Nếu từ không có trong DB nội bộ C++ -> Tra cứu API quốc tế
    if (!result.success && result.error && result.error.includes('không có thật')) {
      const externalDef = await checkWordExternally(cleanWord);
      if (externalDef) {
        // Ghi nhận từ mới vào file dictionary.json dùng chung để C++ nạp lại
        try {
          const fileData = fs.readFileSync(dictPath, 'utf8');
          const dict = JSON.parse(fileData);
          dict[cleanWord] = {
            vi: "Từ tiếng Anh hợp lệ (Tra cứu online).",
            en: externalDef,
            ipa: "/.../",
            category: "others"
          };
          fs.writeFileSync(dictPath, JSON.stringify(dict, null, 2), 'utf8');

          // Chạy lại C++ lần thứ 2 sau khi đã học được từ mới này
          result = await runCpp([
            '--play',
            cleanWord,
            (usedWords || []).join(','),
            botLastChar || '',
            difficulty,
            '15',
            String(timeTaken || 0)
          ]);
        } catch (dbErr) {
          // Lỗi ghi file từ điển thì dùng kết quả lỗi của C++ cũ
        }
      }
    }

    if (!result.success) {
      return res.status(400).json({ error: result.error });
    }

    // 3. Trả kết quả về cho Client React
    return res.json({
      success: true,
      score: result.score,
      isCombo: result.isCombo,
      playerDefinition: result.playerDefinition,
      botWord: result.botWord,
      botDefinition: result.botDefinition,
      updatedUsedWords: result.updatedUsedWords
    });

  } catch (err) {
    return res.status(500).json({ error: 'Lỗi máy chủ Express: ' + err.message });
  }
};

// MINI-GAMES (SCRAMBLE & FILL IN THE BLANK)
export const getMiniGameWord = async (req, res) => {
  try {
    const mode = req.query.mode || req.body.mode || 'scramble';
    const usedWords = req.body.usedWords || [];

    const result = await runCpp([
      '--minigame',
      mode,
      usedWords.join(',')
    ]);

    if (!result.success) {
      return res.status(400).json({ error: result.error });
    }

    // Trả kết quả khớp với mong muốn của client
    if (mode === 'scramble') {
      return res.json({
        word: result.word,
        scrambled: result.scrambled,
        definition: result.definition
      });
    } else {
      return res.json({
        word: result.word,
        blanked: result.blanked,
        definition: result.definition
      });
    }
  } catch (err) {
    return res.status(500).json({ error: 'Lỗi máy chủ Express: ' + err.message });
  }
};

// GỢI Ý (HINT)
export const getHint = async (req, res) => {
  try {
    const { lastChar, usedWords } = req.body;

    if (!lastChar) {
      return res.status(400).json({ error: 'Thiếu chữ cái bắt đầu' });
    }

    const result = await runCpp([
      '--hint',
      lastChar,
      (usedWords || []).join(',')
    ]);

    if (!result.success) {
      return res.status(400).json({ error: result.error });
    }

    return res.json({ hint: result.hint });
  } catch (err) {
    return res.status(500).json({ error: 'Lỗi máy chủ Express: ' + err.message });
  }
};
