export interface GameSettings {
  mode: string;
  difficulty: string;
  timeLimit: number;
  maxAttempts: number;
}

export interface Inventory {
  hint: number;
  revive: number;
  vip: boolean;
}

export interface Flashcard {
  id: number;
  word: string;
  meaning: string | { ipa: string; en: string; vi: string };
}
