<?php

class Droste {

// Constants
#define PI 3.141592653
#define TWOPI 6.283185307
#define PI180 0.017453292
#define I float2(0.0, 1.0)

// Complex math operations
#define complexMult(a,b) float2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x)
#define complexMag(z) float(pow(length(z), 2.0))
#define complexReciprocal(z) float2(z.x / complexMag(z), -z.y / complexMag(z))
#define complexDivision(a,b) complexMult(a, complexReciprocal(b))
#define complexArg(z) float(atan(z.y, z.x))
#define complexLog(z) float2(log(length(z)), complexArg(z))
#define complexExp(z) float2(exp(z.x) * cos(z.y), exp(z.x) * sin(z.y))
#define sinh(x) float((exp(x) - exp(-x)) / 2.0)
#define cosh(x) float((exp(x) + exp(-x)) / 2.0)
#define complexSin(z) float2(sin(z.x) * cosh(z.y), cos(z.x) * sinh(z.y))
#define complexCos(z) float2(cos(z.x) * cosh(z.y), -sin(z.x) * sinh(z.y))
#define complexTan(z) float2(sin(2.0 * z.x)/(cos(2.0 * z.x) + cosh(2.0 * z.y)), sinh(2.0 * z.y)/(cos(2.0 * z.x) + cosh(2.0 * z.y)))
#define complexSinh(z) float2(sinh(z.x) * cos(z.y), cosh(z.x) * sin(z.y))
#define complexCosh(z) float2(cosh(z.x) * cos(z.y), sinh(z.x) * sin(z.y))
#define complexTanh(z) float2(sinh(2.0 * z.x)/(cosh(2.0 * z.a) + cos(2.0 * z.y)), sin(2.0 * z.y)/(cosh(2.0 * z.x) + cos(2.0 * z.y)))
#define polar(r,a) float2(cos(a) * r, sin(a) * r)
#define power(z,p) float2(polar(pow(length(z), float(p)), float(p) * complexArg(z)))

	/** @var float32 			The inner radius of the repeating annular */
	public $radiusInside = 25.0;
    
	/** @var float32 			The outer radius of the repeating annular */
	public $radiusOutside = 100.0;

	/** @var float32 			The number of image the image is repeated on each level */
    public $periodicity = 1.0;
    
    /** @var int32				The number of strands of the spiral */
    public $strands = 1;
    
    /** @var bool				Smoother repeating when using more than one strand */
	public $strandMirror = true;

	/** @var float32			Overall image magnification */
	public $zoom = 0.0;

	/** @var float32			Overall image rotation */
	public $rotate = 0.0;

	/** @var int32    			Super sampling quality. Number of samples squared per pixel */
	public $antialiasing = 1;

    /** @var float32[2]			Panning of the image in the output frame */
    public $center = array(0.0, 0.0);
    
    /** @var float32[2]			Shift the centre of the spiral */
    public $centerShift = array(0.0, 0.0);
    
	/** @var float32[4]			Set the RGBA background colour */
	public $backgroundRGBA = array(0, 0, 0, 1);

	/** @var int32				The number of repeating levels of the spiral */
	public $levels = 9;

	/** @var int32			    The starting spiral level */
	public $levelStart = 3;

	/** @var bool    			Enable for images with transparent middle areas (such as a picture frame) */
	public $transparentInside = true;

	/** @var bool				Enable for images with transparent areas around the outside */
	public $transparentOutside = true;

	/** @var bool				Enable to unroll the circular annular of the image */
	public $twist = true;

	/** @var bool				Automatically set the ideal periodicity for the current radius settings */
    public $periodicityAuto = false;

	/** @var float32			Polar rotation */    
	public $rotatePolar = 0.0;

	/** @var float32			Spin mapped image. Best used with polar rotation */
	public $rotateSpin = 0.0;

	/** @var bool				Enable hyper droste effect */
	public $hyperDroste = false;
	
	/** @var int				Used by hyper droste option */
	public $fractalPoints = 0;
    
    dependent float     r1, r2, p1, p2, w, h, sampleStep, sampleContribution;
    dependent float2    _shift, _center, _rotate, _zoom, xBounds, yBounds, xyMiddle, minDimension;
    dependent float2x2  imageSpin;
    dependent bool tileBasedOnTransparency;
    
	/** 
	* initialize()				Initialize parameters
	*
	* @engine qb 
	* 
	* @local uint32				$i
	* @local float32			$num
	*/
	private function initialize() {
		$this->r1 = $this->radiusInside / 100.0;
		$this->r2 = $this->radiusOutside / 100.0;
		$this->p1 = $this->periodicity;
		if($this->p1 == 0.0) $this->p1 = 0.001;		// Prevent divide by zero
		$this->p2 = $this->strands;
		if($this->p2 == 0.0) $this->p2 = 0.0001;
		$this->tileBasedOnTransparency = ($this->transparentInside || !$this->transparentOutside);
		
		$this->_shift = 1.0 + $this->centerShift / 100.0;
		$this->_center = ($this->size / 2.0) + $this->center * ($this->size / 2.0) / 100.0;
		
		$this->w = $this->size[0];
		$this->h = float(size.y);
		minDimension = float2(min(w, h) / 2.0);
		
		// Autoset periodicity
		if (periodicityAuto) {
		p1 = p2/2.0 * (1.0 + sqrt(1.0 - pow(log(r2/r1)/PI, 2.0)));
		}
		
		// Set rotation
		_rotate = p1 > 0.0 ? float2(-PI180 * rotate, 0.0) : float2(PI180 * rotate, 0.0);
		
		float sc = cos(radians(rotateSpin));
		float ss = sin(radians(rotateSpin));
		imageSpin = float2x2(sc, ss, -ss, sc);
		
		// Set zoom
		_zoom = float2((exp(zoom) + radiusInside - 1.0) / 100.0, 0.0);
		
		// Scale viewport pixels to complex plane
		if($this->twist) {
			$this->xBounds = float2(-r2, r2);
			$this->yBounds = float2(-r2, r2);
		} else {
			$this->xBounds = float2(-log(r2/r1), log(r2/r1));
			$this->yBounds = float2(0.0, 2.1 * PI);
		}
		
		xyMiddle = float2(xBounds.x + xBounds.y, yBounds.x + yBounds.y) / 2.0;
		float2 xyRange  = float2(xBounds.y - xBounds.x, yBounds.y - yBounds.x);
		xyRange.x = xyRange.y * (w / h);
		xBounds = float2(xyMiddle.x - xyRange.x/2.0, xyMiddle.x + xyRange.x/2.0);
		
		// Super sampling
		$this->sampleStep = 1.0 / $this->antialiasing;
		$this->sampleContribution = 1.0 / pow($this->antialiasing, 2.0);
	
    }
    
    private function render($z, &$alphaRemaining, &$sign, &$iteration, &$colorSoFar) {
        float2 d = minDimension * (z + _shift);
        sign = 0;
        
        if($this->tileBasedOnTransparency || iteration == 0) {
            pixel4 color = sample(src, d);
            $colorSoFar += $color * ($color->a * $alphaRemaining);
            $alphaRemaining *= (1.0 - $colorSoFar->a);
        }
        
        if($this->tileBasedOnTransparency) {
            if (!$this->transparentOutside && $alphaRemaining > 0.0) $sign = -1;
            if ($this->transparentOutside && $alphaRemaining > 0.0) $sign = 1;
        } else {
            if ($iteration > 0) $colorSoFar = sample(src, d);
            $radius = length($z);
            $sign = ($radius < $this->r1) ? -1 : ($radius > $this->r2 ? 1 : 0);
        }
        $iteration += 1;
    }
    
	private function renderPoint($s) {
        float2 z, d, ratio, polar;
        float radius, theta, div;
        int iteration;
        int sign = 0;
        float alphaRemaining = 1.0;
        pixel4 colorSoFar = pixel4(0, 0, 0, 0);
        
        $z->x = xBounds.x + (xBounds.y - xBounds.x) * ((s.x - _center.x) + $this->w / 2.0) / $this->w;
        $z->y = yBounds.x + (yBounds.y - yBounds.x) * ((s.y - _center.y) + $this->h / 2.0) / $this->h;
        
        // Only allow for procedural zooming/scaling in the standard coordinates
        if (twist) {
            z = xyMiddle + complexMult(complexDivision((z - xyMiddle), _zoom), complexExp(complexMult(-I, _rotate)));
        }
        
        // Extra options
        if (hyperDroste) {
            z = complexSin(z);
        }
        
        if (fractalPoints > 0) {
            z = power(z, fractalPoints);
            z = complexTan(complexMult(z, float2(2.0, 0.0)));
        }
        
        if (rotatePolar != 0.0) {
            theta = PI180 * rotatePolar;
            
            div = (1.0 + pow(z.x, 2.0) + pow(z.y, 2.0) + ((1.0 - pow(z.x, 2.0) - pow(z.y, 2.0)) * cos(theta)) - (2.0 * z.x * sin(theta))) / 2.0;
            z.x = z.x * cos(theta) + ((1.0 - pow(z.x, 2.0) - pow(z.y, 2.0)) * sin(theta) / 2.0);
            z = complexDivision(z, float2(div, 0.0));
        }
        
        z *= imageSpin;
        
        if (twist) {
            z = complexLog(complexDivision(z, float2(r1, 0.0)));
        }
        
        // Start Droste-effect code
        float2 alpha = float2(atan((p2/p1) * (log(r2/r1) / TWOPI)), 0.0);
        float2 f = float2(cos(alpha.x), 0.0);
        float2 beta = complexMult(f, complexExp(complexMult(alpha, I)));
        
        // The angle of rotation between adjacent annular levels
        float2 angle = float2(-TWOPI * p1, 0.0);
        
        if (p2 > 0.0) angle = -angle;
        if (strandMirror) angle /= p2;
        
        z = complexDivision(complexMult(float2(p1, 0.0), z), beta);
        z = complexMult(float2(r1, 0.0), complexExp(z));
        // End Droste-effect code
        
        
        // Start drawing
        if (tileBasedOnTransparency && levelStart > 0) {
            if (transparentOutside) {
				ratio = complexMult(float2(r2/r1, 0.0), complexExp(complexMult(angle, I)));
			} else {
            	ratio = complexMult(float2(r1/r2, 0.0), complexExp(complexMult(angle,-I)));
			}
            z = complexMult(z, power(ratio, levelStart));
        }
        
        iteration = 0;
        render(z, alphaRemaining, sign, iteration, colorSoFar);
        
        if (sign < 0) ratio = complexMult(float2(r2/r1, 0.0), complexExp(complexMult(angle, I)));
        if (sign > 0) ratio = complexMult(float2(r1/r2, 0.0), complexExp(complexMult(angle, -I)));
        
        iteration = levelStart;
        int maxIteration = levels + levelStart - 1;
        
        while (sign != 0 && iteration < maxIteration) {
            z = complexMult(z, ratio);
            render(z, alphaRemaining, sign, iteration, colorSoFar);
        }
        
        return colorSoFar;
    
    }
    
    
    void evaluatePixel()
    {
        pixel4 c = backgroundRGBA;
        
        if (antialiasing > 1) {
            // Average detailSuperSample^2 points per pixel
            for (float i = 0.0; i < 1.0; i += sampleStep)
                for (float j = 0.0; j < 1.0; j += sampleStep)
                    c += sampleContribution * renderPoint(float2(outCoord().x + i, outCoord().y + j));
        } else {
           c = renderPoint(outCoord());
        }
        
        if (c.a < 1.0) c = mix(c, backgroundRGBA, 1.0 - c.a);
        
        dst = c;
    }
}
