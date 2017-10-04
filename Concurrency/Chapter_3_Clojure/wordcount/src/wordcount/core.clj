(ns wordcount.core)

(defn count-words-sequential [pages]
  (frequencies (mapcat get-words pages)))

(defn get-words [text]
  (re-seq #"\w+" text))
