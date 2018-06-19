package com.mrpoid.core;

import android.content.Context;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.os.Vibrator;


/**
 * 声音播放管理器
 * 
 * @author Yichou 2013-12-13 
 * 
 * <p>优化代码结构 2013-12-13 0:03:33
 *
 */
public final class EmuAudio implements OnErrorListener, OnCompletionListener, MrDefines {
	private static final String TAG = "EmuAudio";
	
	//meidia 接口编号区
	public static final int MR_MEDIA_INIT = 201, 
		MR_MEDIA_BUF_LOAD = 203, 
		MR_MEDIA_FILE_LOAD = 202, 
		MR_MEDIA_PLAY_CUR_REQ = 204, 
		MR_MEDIA_PAUSE_REQ = 205, 
		MR_MEDIA_RESUME_REQ = 206,
		MR_MEDIA_STOP_REQ = 207, 
		MR_MEDIA_CLOSE = 208, 
		MR_MEDIA_GET_STATUS = 209, 
		MR_MEDIA_SETPOS = 210, 
		MR_MEDIA_GETTIME = 211, 
		MR_MEDIA_GET_TOTAL_TIME = 212, 
		MR_MEDIA_GET_CURTIME = 213,
		MR_MEDIA_GET_CURTIME_MSEC = 215, 
		MR_MEDIA_FREE = 216, 
		MR_MEDIA_ALLOC_INRAM = 220, 
		MR_MEDIA_FREE_INRAM = 221,
		MR_MEDIA_OPEN_MUTICHANNEL = 222, 
		MR_MEDIA_PLAY_MUTICHANNEL = 223,
		MR_MEDIA_STOP_MUTICHANNEL = 224, 
		MR_MEDIA_CLOSE_MUTICHANNEL = 225;
	
	private MediaPlayer mp3Player;
	private MediaPlayer mediaPlayer;
	private boolean audioPaused = false;
	private Emulator emulator;
	private Vibrator vibrator;
	private boolean recyled = false;
	
	private boolean needCallback = false;
	private int pausePosition;
	private int stat = MrDefines.MR_MEDIA_IDLE;
	

	public EmuAudio(Emulator emulator) {
		this.emulator = emulator;
		
		vibrator = (Vibrator)emulator.getContext().getSystemService(Context.VIBRATOR_SERVICE);
		mp3Player = new MediaPlayer();
//		float v = Prefer.volume/100.0f;
//		mp3Player.setVolume(v, v);
	}
	
	public boolean isRecyled() {
		return recyled;
	}
	
	public synchronized void recyle() {
		if(recyled)
			return;
		
		if(mp3Player != null) {
			mp3Player.release();
			mp3Player = null;
		}
		
		if(mediaPlayer != null) {
			mediaPlayer.release();
			mediaPlayer = null;
		}
		
		if(vibrator != null){
			vibrator.cancel();
			vibrator = null;
		}
		
		recyled = true;
	}
	
	public void pause() {
		if(recyled) return;
		
		if(mp3Player.isPlaying()) {
			audioPaused = true;
			mp3Player.pause();
		}
		
		//多媒体接口不暂停
//		if(mediaPlayer != null)
//			N2J_musicCMD(MR_MEDIA_PAUSE_REQ, 0, 0);
		vibrator.cancel();
	}
	
	public void resume() {
		if(recyled) return;
		
		if(audioPaused) {
			audioPaused = false;
			mp3Player.start();
		}

		//多媒体接口不暂停
//		if(mediaPlayer != null)
//			N2J_musicCMD(MR_MEDIA_RESUME_REQ, 0, 0);
	}
	
	public void stop() {
		if(recyled) return;
		
		if(mp3Player.isPlaying()) {
			mp3Player.stop();
			mp3Player.reset();
		}
		
		if(mediaPlayer != null) {
			mediaPlayer.release();
			mediaPlayer = null;
		}
	}
	
	@Override
	public boolean onError(MediaPlayer mp, int what, int extra) {
		EmuLog.e(TAG, String.format("onError(%d, %d)", what, extra));
		if(needCallback){
			emulator.native_callback(0x1001, 1); //ACI_PLAY_ERROR       1  //播放时遇到错误
		}
		
		return false;
	}
	
	@Override
	public void onCompletion(MediaPlayer mp) {
		EmuLog.i(TAG, "onCompletion");
		
		stat = MR_MEDIA_LOADED; //播放完成后的状态
		if(needCallback){
			emulator.native_callback(0x1001, 0); //ACI_PLAY_COMPLETE   0  //播放结束
		}
	}

	public void N2J_playSound(String path, int loop) {
		if(recyled) return;
		
		if(path == null) return;

		if(mp3Player == null)
			mp3Player = new MediaPlayer();
		
		EmuLog.i(TAG, "play sound :" + path);
		
		mp3Player.stop();
		mp3Player.reset();
		try {
			mp3Player.setDataSource(path);
			mp3Player.prepare();
			mp3Player.setLooping(loop==1);
			mp3Player.start();
			audioPaused = false;
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void N2J_stopSound() {
		if(recyled) return;
		
		EmuLog.i(TAG, "stop sound");
		
		if(mp3Player.isPlaying()){
			mp3Player.stop();
			mp3Player.reset();
		}
	}
	
	public void N2J_musicLoadFile(String path) {
		if(recyled) return;
		
		if(mediaPlayer == null || path == null) 
			return;

		EmuLog.d(TAG, "musicLoadFile:" + path);
		
		try {
			mediaPlayer.setDataSource(path);
			mediaPlayer.prepare(); //同步
			stat = MrDefines.MR_MEDIA_LOADED;
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public int N2J_musicCMD(int cmd, int arg0, int arg1) {
		int ret = 0;
		
		if(recyled) return ret;
		
		if(cmd == MR_MEDIA_INIT) {
			if(mediaPlayer != null){
				mediaPlayer.release();
				mediaPlayer = null;
			}

			mediaPlayer = new MediaPlayer();
			mediaPlayer.setOnErrorListener(this);
			mediaPlayer.setOnCompletionListener(this);
			stat = MrDefines.MR_MEDIA_INITED;
			
			return 0;
		}
		
		if(mediaPlayer == null) 
			return 0;

		switch (cmd) 
		{
		case MR_MEDIA_PLAY_CUR_REQ: {//204
			if (stat == MrDefines.MR_MEDIA_LOADED) {
				stat = MrDefines.MR_MEDIA_PLAYING;
			} else if (stat == MrDefines.MR_MEDIA_PAUSED) {
				mediaPlayer.seekTo(pausePosition);
				stat = MrDefines.MR_MEDIA_PLAYING;
			}
			
			if (arg1 == 1) { //为 1 则需要播放回调函数
				EmuLog.i(TAG, "need callback");
				
				needCallback = true;
				mediaPlayer.setLooping(arg0==1);
			}else {
				needCallback = false;
			}
			
			mediaPlayer.start();
			mediaPlayer.setOnCompletionListener(this);
			
			break;
		}
		
		case MR_MEDIA_PAUSE_REQ: {//205
			if (stat == MR_MEDIA_PLAYING) {
				pausePosition = mediaPlayer.getCurrentPosition();
				mediaPlayer.pause();
				stat = MR_MEDIA_PAUSED;
			}
			break;
		}
		
		case MR_MEDIA_RESUME_REQ: {//206
			if (stat == MR_MEDIA_PAUSED) {
				mediaPlayer.seekTo(pausePosition);
				mediaPlayer.start();
				stat = MR_MEDIA_PLAYING;
			}
			break;
		}
		
		case MR_MEDIA_STOP_REQ: {//207
			mediaPlayer.stop();
			stat = MR_MEDIA_LOADED;
			break;
		}
		
		case MR_MEDIA_CLOSE: {//208
			mediaPlayer.stop();
			mediaPlayer.reset();
			stat = MR_MEDIA_IDLE;
			break;
		}

		case MR_MEDIA_GET_STATUS://209
			ret = stat;
			break;

		case MR_MEDIA_SETPOS://210
			mediaPlayer.seekTo(arg0);
			break;

		case MR_MEDIA_GET_TOTAL_TIME://212
			ret = mediaPlayer.getDuration()/1000; //s
			break;
		case MR_MEDIA_GET_CURTIME://213
			ret = mediaPlayer.getCurrentPosition() / 1000;
			break;
		case MR_MEDIA_GET_CURTIME_MSEC://215
			ret = mediaPlayer.getCurrentPosition();
			break;

		case MR_MEDIA_FREE: {//216
			mediaPlayer.release();
			mediaPlayer = null;
			stat = MR_MEDIA_NULL;
			break;
		}

		case 1302: // 设置音量
			mediaPlayer.setVolume(arg0 / 5.0f, arg0 / 5.0f);
			break;
			
		default:
			ret = 0;
			break;
		}

//		EmuLog.d(TAG, String.format("musicCMD(%d,%d,%d) ret=%d", cmd, arg0, arg1, ret));

		return ret;
	}

	public void N2J_startShake(int ms){
		if(recyled) return;
		
		vibrator.vibrate(ms);
    }
	
	public void N2J_stopShake(){
		if(recyled) return;
		
		vibrator.cancel();
	}
}
