(batch* electron-glow.clp)

(loop-for-count (?j 0 1000) do
                (loop-for-count (?k 0 100) do 
                                (electron-glow-ring (electron-glow-normalize ring (random))
                                                    (- 100 ?k))
                                (electron-glow-delay 5)))
