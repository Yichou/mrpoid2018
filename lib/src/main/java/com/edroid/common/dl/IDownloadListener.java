package com.edroid.common.dl;

public interface IDownloadListener {
	/**
	 * 开始下载前检测到本地存在同名文件，则回调此方法，（下载线程）<br />
	 * 调用者返回下面几个参数中的一个，下载器做出不同处理
	 * 
	 * @return 
	 * 	<ol>0 （默认）直接回调成功 onFinish()</ol>
	 * 	<ol>1 删除本地文件，重新下载</ol>
	 */
	public int onExist();
	
	/**
	 * 由于当前下载任务超数，下载添加到队列，回调此方法
	 */
	public void onWait();
	
	/**
	 * 从服务器成功获取到文件长度后回调（下载线程）<br />
	 * 可在此方法反馈值，告诉下载器是否继续下载
	 * 
	 * @param total 总长
	 * @param remain 剩余长度（断点）
	 * 
	 * @return
	 * 	<ol>0 （默认）继续开始下载</ol>
	 * 	<ol>1 不下载，回调 onCancel() </ol>
	 */
	public int onLengthGet(long total, long remain);
	
	/**
	 * 开始下载
	 * 
	 * @param total 总字节数
	 */
	public void onStart(long start, long total);
	
	/**
	 * 下载过程回调
	 * 
	 * @param cur 已下载字节数
	 * @param prog 进度 0~100
	 */
	public void onProgress(long cur, byte prog);
	
	/**
	 * 下载取消 
	 * 
	 * Downloader.cancel() 触发，或者 onLengthGet() 回调
	 */
	public void onCancel();
	
	/**
	 * 下载完成
	 */
	public void onFinish();
	
	/**
	 * 下载过程出异常
	 * 
	 * @param msg
	 */
	public void onError(String msg, Exception e);
	
//	public void onRetry();

	/**
	 * 下载器关闭的时候，无论如何都会被调用
	 */
	public void onClose();
}
