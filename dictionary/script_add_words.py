import json
import urllib.request
import nltk
from nltk.corpus import wordnet
import eng_to_ipa as ipa
from deep_translator import GoogleTranslator
import time

def setup():
    nltk.download('wordnet')
    nltk.download('omw-1.4')

def get_common_words():
    url = "https://raw.githubusercontent.com/first20hours/google-10000-english/master/google-10000-english-no-swears.txt"
    req = urllib.request.Request(url)
    with urllib.request.urlopen(req) as response:
        words = response.read().decode('utf-8').splitlines()
    return words

def determine_category(synset):
    lexname = synset.lexname()
    if lexname == 'noun.animal': return 'animals'
    if lexname == 'noun.plant': return 'plants'
    if lexname == 'noun.artifact': return 'objects'
    if lexname == 'noun.food': return 'food'
    if lexname == 'noun.body': return 'body'
    return 'others'

def main():
    setup()
    
    dict_path = 'dictionary.json'
    with open(dict_path, 'r', encoding='utf-8') as f:
        dictionary = json.load(f)
        
    print(f"Original dictionary size: {len(dictionary)}")
    
    words = get_common_words()
    translator = GoogleTranslator(source='en', target='vi')
    
    added_count = 0
    batch_size = 50
    words_to_process = []
    
    for w in words:
        if len(w) < 3: continue
        if w in dictionary: continue
        
        synsets = wordnet.synsets(w)
        if not synsets: continue
        
        synset = synsets[0]
        if '_' in w or '-' in w: continue
        
        words_to_process.append((w, synset))
            
    print(f"Found {len(words_to_process)} new words to potentially add.")
    
    for i in range(0, len(words_to_process), batch_size):
        batch = words_to_process[i:i+batch_size]
        if not batch: break
        
        text_to_translate = "\n".join([w for w, _ in batch])
        try:
            translations = translator.translate(text_to_translate).split('\n')
        except Exception as e:
            print(f"Translation failed: {e}")
            time.sleep(2)
            continue
            
        for j, (w, synset) in enumerate(batch):
            if j >= len(translations): break
            vi_meaning = translations[j]
            
            en_meaning = synset.definition()
            en_meaning = en_meaning[:1].upper() + en_meaning[1:]
            category = determine_category(synset)
            
            word_ipa = ipa.convert(w)
            if '*' in word_ipa:
                word_ipa = w
            
            ipa_str = f"/{word_ipa}/"
            
            dictionary[w] = {
                "vi": vi_meaning.strip().capitalize(),
                "en": en_meaning,
                "ipa": ipa_str,
                "category": category
            }
            added_count += 1
                
        print(f"Added {added_count} words so far...")
            
    with open(dict_path, 'w', encoding='utf-8') as f:
        json.dump(dictionary, f, ensure_ascii=False, indent=2)
        
    print(f"Done! Added {added_count} words. New dictionary size: {len(dictionary)}")

if __name__ == '__main__':
    main()
