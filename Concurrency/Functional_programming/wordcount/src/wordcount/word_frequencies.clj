(ns wordcount.word-frequencies)
(defn word-frequencies [words]
  (reduce (fn [counts word] (assoc counts word (inc (get counts word 0)))) {} words))
