Import('env')
from os.path import join, realpath

halmap = {
	"espressif32" : "esp32",
	"espressif8266" : "esp8266",
}

pioPlatform = env.get("PIOPLATFORM")
if pioPlatform in halmap:
	halPf = halmap.get(pioPlatform)
	halBuildFlags = [(halPf.upper(), 1)]
	halSrcFilter = ["+<*> -<hal/*>", "+<hal/**/%s>" % halPf]
	print("building HAL for: %s with flags %s and source filter %s" % (halPf, halBuildFlags, halSrcFilter))
	env.Append(CPPDEFINES=halBuildFlags)
	env.Replace(SRC_FILTER=halSrcFilter)
