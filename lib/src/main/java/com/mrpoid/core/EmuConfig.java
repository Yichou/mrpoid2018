package com.mrpoid.core;

/**
 * @author yichou 2018-6-15
 */
public class EmuConfig {
    public static final int SCALE_ORIGINAL = 0, SCALE_STRE = 1, SCALE_2X = 2, SCALE_PRO = 3;

    public int scnw;
    public int scnh;
    public int heapSize;
    public int scaleMode;
    public int padAlpha;
    public boolean anti;
    public boolean sysfont;
    public boolean catchMenuButton;
    public boolean catchVolumeButton;
    public boolean enableKeyVirb;
    public int sysfontSize;
    public int bgColor;

    public EmuConfig() {
        scnw = 240;
        scnh = 320;
        heapSize = 4096;
        scaleMode = SCALE_PRO;
        anti = true;
        sysfont = false;
        bgColor = 0xfff0f0f0;
        padAlpha = 0x80;
        enableKeyVirb = true;
    }

    private static EmuConfig instance;

    public static EmuConfig getInstance() {
        if(instance == null)
            instance = new EmuConfig();
        return instance;
    }
}


