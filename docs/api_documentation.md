# VocabMaster API Documentation

Tài liệu này cung cấp thông tin chi tiết về các API endpoints được cung cấp bởi server của VocabMaster. Base URL cho các API này tùy thuộc vào cấu hình môi trường triển khai (thường là `http://localhost:5000` khi chạy local).

---

## 1. Game Play (Chơi game nối chữ)

API này xử lý lượt đi của người chơi, kiểm tra tính hợp lệ của từ và trả về nước đi tiếp theo của Bot.

- **Endpoint:** `/api/game/play`
- **Method:** `POST`
- **Content-Type:** `application/json`

### Request Body

| Trường | Kiểu dữ liệu | Bắt buộc | Mô tả |
| :--- | :--- | :--- | :--- |
| `word` | string | Có | Từ tiếng Anh mà người chơi nhập vào. |
| `usedWords` | array[string] | Không | Danh sách các từ đã được sử dụng trong ván chơi hiện tại để tránh trùng lặp. Mặc định là mảng rỗng `[]`. |
| `botLastChar` | string | Không | Ký tự cuối cùng của từ mà Bot đã đánh ở lượt trước. Dùng để kiểm tra xem từ của người chơi có nối đúng chữ không. |
| `difficulty` | string | Không | Độ khó của Bot (`easy`, `medium`, `hard`). Mặc định là `easy`. |
| `timeTaken` | number | Không | Thời gian người chơi dùng để suy nghĩ (tính bằng giây). Dùng để tính điểm thưởng. |

**Ví dụ Request:**
```json
{
  "word": "apple",
  "usedWords": [],
  "botLastChar": "",
  "difficulty": "medium",
  "timeTaken": 5
}
```

### Response (Thành công - 200 OK)

| Trường | Kiểu dữ liệu | Mô tả |
| :--- | :--- | :--- |
| `success` | boolean | `true` xác nhận lượt đi hợp lệ và thành công. |
| `score` | number | Điểm số người chơi đạt được trong lượt đi này. |
| `isCombo` | boolean | Cờ đánh dấu người chơi có đạt Combo (trả lời nhanh hoặc dùng từ khó) hay không. |
| `playerDefinition` | string | Nghĩa của từ người chơi vừa nhập. |
| `botWord` | string | Từ tiếng Anh Bot dùng để phản hồi lại. |
| `botDefinition` | string | Nghĩa của từ Bot vừa đánh. |
| `updatedUsedWords` | array[string] | Danh sách từ đã sử dụng mới nhất (bao gồm cả từ của người chơi và Bot). |

**Ví dụ Response:**
```json
{
  "success": true,
  "score": 15,
  "isCombo": false,
  "playerDefinition": "quả táo",
  "botWord": "elephant",
  "botDefinition": "con voi",
  "updatedUsedWords": ["apple", "elephant"]
}
```

### Response (Lỗi - 400 Bad Request / 500 Internal Server Error)

```json
{
  "error": "Từ 'apple' đã được sử dụng trước đó!"
}
```

---

## 2. Mini Game (Trò chơi phụ)

Sinh ra một từ vựng để người chơi giải đố trong các Mini game như "Xếp lại từ" (Scramble) hoặc "Điền vào chỗ trống" (Fill in the blank).

- **Endpoint:** `/api/game/minigame`
- **Method:** `POST`
- **Content-Type:** `application/json`

### Query Parameter / Request Body

| Trường | Kiểu dữ liệu | Bắt buộc | Mô tả |
| :--- | :--- | :--- | :--- |
| `mode` | string | Không | Chế độ chơi: `scramble` hoặc `fill_in_the_blank` (có thể truyền qua query parameter `?mode=...` hoặc body). Mặc định là `scramble`. |
| `usedWords` | array[string] | Không | Danh sách các từ cần loại trừ (đã xuất hiện) để tránh lặp từ. |

**Ví dụ Request:**
```json
{
  "mode": "scramble",
  "usedWords": ["apple", "elephant"]
}
```

### Response (Chế độ `scramble`)

```json
{
  "word": "banana",
  "scrambled": "nabana",
  "definition": "quả chuối"
}
```

### Response (Chế độ `fill_in_the_blank`)

```json
{
  "word": "orange",
  "blanked": "o_an_e",
  "definition": "quả cam / màu cam"
}
```

---

## 3. Hint (Gợi ý)

Cung cấp cho người chơi một từ gợi ý hợp lệ bắt đầu bằng ký tự được yêu cầu.

- **Endpoint:** `/api/game/hint`
- **Method:** `POST`
- **Content-Type:** `application/json`

### Request Body

| Trường | Kiểu dữ liệu | Bắt buộc | Mô tả |
| :--- | :--- | :--- | :--- |
| `lastChar` | string | Có | Ký tự bắt buộc mà từ gợi ý phải bắt đầu bằng. |
| `usedWords` | array[string] | Không | Danh sách từ đã sử dụng để gợi ý không bị trùng lặp. |

**Ví dụ Request:**
```json
{
  "lastChar": "e",
  "usedWords": ["apple", "elephant"]
}
```

### Response (Thành công - 200 OK)

```json
{
  "hint": "eagle"
}
```

### Response (Lỗi - 400 Bad Request)

```json
{
  "error": "Thiếu chữ cái bắt đầu"
}
```

---

## 4. Health Check (Kiểm tra trạng thái)

API dùng để kiểm tra xem server có đang hoạt động bình thường không.

- **Endpoint:** `/api/health`
- **Method:** `GET`

### Response (Thành công - 200 OK)

```json
{
  "status": "ok",
  "message": "Server is running C++ Engine"
}
```
