(batch* electron-glow.clp)

; Run this a bunch of times...watch the magic happen :D
; Will look like a multicolored flux capacitor :D
(defglobal MAIN 
           ?*delay* = 3 
           ?*brightness-max* = 64)
(electron-glow-off)
(loop-for-count (?j 0 10) do
                (loop-for-count (?i 0 5) do
                                (loop-for-count (?k 0 ?*brightness-max*) do
                                                (electron-glow-ring ?i ?k)
                                                (electron-glow-delay ?*delay*)))
                (loop-for-count (?i 0 5) do
                                (loop-for-count (?k 0 ?*brightness-max*) do
                                                (electron-glow-ring ?i (- ?*brightness-max* ?k))
                                                (electron-glow-delay ?*delay*))))
