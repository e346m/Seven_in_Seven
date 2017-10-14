(ns wordcount.core)

(defn count-words-sequential [pages]
  (frequencies (mapcat get-words pages)))

(defn get-words [text]
  (re-seq #"\w+" text))

(defn count-words-parallel [pages]
  (reduce (partial merge-with +)
    (pmap #(frequencies (get-words %)) pages)))

(defn count-words [pages]
  (reduce (partial merge-with +)
      (pmap count-words-sequential (partition-all 100 pages)))

