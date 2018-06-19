package com.edroid.common.dl;

public abstract class DownloadListener implements IDownloadListener {
	
	@Override
	public int onExist() {
		return 0;
	}
	
	/**
	 * do not call super
	 */
	@Override
	public void onWait() {
	}
	
	@Override
	public int onLengthGet(long total, long remain) {
		return 0;
	}
	
	/**
	 * do not call super
	 */
	@Override
	public void onStart(long start, long total) {
	}

	/**
	 * do not call super
	 */
	@Override
	public void onProgress(long cur, byte prog) {
	}

	/**
	 * do not call super
	 */
	@Override
	public void onCancel() {
	}

	/**
	 * do not call super
	 */
	@Override
	public void onFinish() {
	}

	/**
	 * do not call super
	 */
	@Override
	public void onError(String msg, Exception e) {
	}
	
	/**
	 * do not call super
	 */
	@Override
	public void onClose() {
	}
}
