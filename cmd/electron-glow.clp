; NOTE: This only works with the piglow on the raspberry pi
; END OF LINE!
(defgeneric electron-glow-normalize)
(defgeneric electron-glow-color)
(defgeneric electron-glow-on)
(defgeneric electron-glow-off)

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type leg))
   (?value INTEGER (or (> 0 ?value)
                       (> ?value 2))))
  (mod (abs ?value) 3))

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type leg))
   (?value INTEGER (<= 0 ?value 2)))
  ?value)

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type ring))
   (?value INTEGER (or (> 0 ?value)
                       (> ?value 5))))
  (mod (abs ?value) 6))

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type ring))
   (?value INTEGER (<= 0 ?value 5)))
  ?value)

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type intensity))
   (?value INTEGER (or (> 0 ?value)
                       (> ?value 255))))
  (mod (abs ?value) 256))

(defmethod electron-glow-normalize
  ((?type SYMBOL (eq ?type intensity))
   (?value INTEGER (<= 0 ?value 255)))
  ?value)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color red RED Red))))
  0)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color orange ORANGE Orange))))
  1)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color yellow YELLOW Yellow))))
  2)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color green GREEN Green))))
  3)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color blue BLUE Blue))))
  4)

(defmethod electron-glow-color
  ((?color SYMBOL (not (neq ?color white WHITE White))))
  5)

(defmethod electron-glow-one 
  ((?leg INTEGER)
   (?ring SYMBOL)
   (?intensity INTEGER))
  (electron-glow-one (electron-glow-normalize leg ?leg)
              (electron-glow-color ?ring)
              (electron-glow-normalize intensity ?intensity)))

(defmethod electron-glow-one
  ((?leg INTEGER)
   (?ring SYMBOL)
   (?intensity SYMBOL (eq ?intensity off)))
  (electron-glow-one ?leg ?ring 0))

(defmethod electron-glow-ring
  ((?ring SYMBOL)
   (?intensity INTEGER))
  (electron-glow-ring (electron-glow-color ?ring)
               (electron-glow-normalize intensity ?intensity)))

(defmethod electron-glow-ring
  ((?ring SYMBOL)
   (?intensity SYMBOL (eq ?intensity off)))
  (electron-glow-ring ?ring 0))

(defmethod electron-glow-leg
  ((?leg INTEGER)
   (?intensity SYMBOL (eq ?intensity off)))
  (electron-glow-leg (electron-glow-normalize leg ?leg) 0))

(defmethod electron-glow-off
  "Turns off all leds"
  ()
  (electron-glow-leg 0 off)
  (electron-glow-leg 1 off)
  (electron-glow-leg 2 off))

(defmethod electron-glow-on
  "Turns on all leds at a specified value"
  ((?intensity INTEGER))
  (bind ?nint (electron-glow-normalize intensity ?intensity))
  (electron-glow-leg 0 ?nint)
  (electron-glow-leg 1 ?nint)
  (electron-glow-leg 2 ?nint))
