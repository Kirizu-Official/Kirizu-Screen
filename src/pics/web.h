#include <pgmspace.h>

const char WEB_FILE_INDEX[] PROGMEM = R"delimiter(
<!DOCTYPE html>
<html lang="zh-cmn-Hans">
    <head>
        <meta charset="utf-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, shrink-to-fit=no" />
        <meta name="renderer" content="webkit" />
        <meta name="force-rendering" content="webkit" />
        <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />

        <!-- MDUI CSS -->
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/mdui@1.0.2/dist/css/mdui.min.css" />
        <title>Kirizu 管理页面</title>
    </head>

    <body class="mdui-appbar-with-toolbar mdui-theme-primary-indigo mdui-theme-accent-pink mdui-theme-layout-auto">
        <header class="appbar mdui-appbar mdui-appbar-fixed">
            <div class="mdui-toolbar mdui-color-theme">
                <a href="./" class="mdui-typo-title">Kirizu - 设置</a>
                <div class="mdui-toolbar-spacer"></div>
                <button
                    class="mdui-btn mdui-btn-icon mdui-ripple mdui-ripple-white"
                    mdui-tooltip="{content: '重启 Kirizu'}"
                    onclick="setConfig('restart',true)">
                    <i class="mdui-icon material-icons">refresh</i>
                </button>
            </div>
        </header>
        <div class="mdui-container mdui-p-b-4">
            <div class="mdui-row">
                <div class="mdui-col-xs-12 mdui-col-md-6">
                    <div class="mdui-card mdui-m-t-3">
                        <div class="mdui-card-primary">
                            <div class="mdui-card-primary-title">图片管理</div>
                            <div class="mdui-card-primary-subtitle">注：图片解析状态只会在图片解析时刷新</div>
                        </div>
                        <div class="mdui-card-content">
                            <div class="mdui-table-fluid">
                                <table class="mdui-table mdui-table-hoverable">
                                    <tbody>
                                        <tr>
                                            <td>打印机连接</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/connect.gif">connect.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_connect"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('connect')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>打印机报错</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/error.gif">error.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_error"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('error')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>打印机待机</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/wait.gif">wait.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_wait"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('wait')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-归位</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/homing.gif">homing.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_homing"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('homing')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>打印-Z轴对齐</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/ztile.gif">ztile.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_ztile"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('ztile')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-床网</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/bedmesh.gif">bedmesh.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_bedmesh"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('bedmesh')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-热床加热</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/bedheat.gif">bedheat.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_bedheat"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('bedheat')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-挤出加热</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/extruderheat.gif">extruderheat.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_extruderheat"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('extruderheat')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-准备打印</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/printable.gif">printable.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_printable"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('printable')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>

                                        <tr>
                                            <td>打印-打印完成</td>
                                            <td class="mdui-typo">
                                                <a target="_blank" href="/download/printdone.gif">printdone.gif</a>
                                                <i
                                                    class="mdui-icon material-icons mdui-text-color-theme-accent"
                                                    mdui-tooltip="{content: '图片出现解析异常，请重新上传该图片'}"
                                                    id="ico_printdone"
                                                    >error</i
                                                >
                                            </td>
                                            <td>
                                                <button
                                                    class="mdui-btn mdui-ripple mdui-color-theme-accent"
                                                    onclick="uploadStart('printdone')">
                                                    上传
                                                </button>
                                            </td>
                                        </tr>
                                    </tbody>
                                </table>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="mdui-col-xs-12 mdui-col-md-6">
                    <div class="mdui-card mdui-m-t-3">
                        <div class="mdui-card-primary">
                            <div class="mdui-card-primary-title">
                                <span class="mdui-chip-icon mdui-m-r-1" id="color-show"></span>颜色控制
                            </div>
                            <div class="mdui-card-primary-subtitle mdui-m-t-1">
                                设置 Kirizu 显示的主色（如温度、打印进度等颜色）
                            </div>
                        </div>
                        <div class="mdui-card-content" style="text-align: center">
                            <div id="color-picker"></div>
                        </div>
                    </div>

                    <div class="mdui-card mdui-m-t-3">
                        <div class="mdui-card-primary mdui-p-b-0">
                            <div class="mdui-card-primary-title">参数设置</div>
                            <div class="mdui-card-primary-subtitle">保存配置后建议手动重启 Kirizu</div>
                        </div>

                        <div class="mdui-card-content">
                            屏幕亮度：
                            <label class="mdui-slider mdui-slider-discrete" style="line-height: 1.15">
                                <input type="range" step="1" min="0" max="255" id="input_brightness" />
                            </label>
                        </div>
                        <div class="mdui-card-content mdui-p-t-0">
                            <div class="mdui-textfield">
                                <label class="mdui-textfield-label">Klipper IP：</label
                                ><input class="mdui-textfield-input" type="text" id="input_klipperip" />
                                <div class="mdui-textfield-helper">Klipper 的IP地址，无需加 http:// 和 /</div>
                            </div>
                        </div>
                        <div class="mdui-card-actions mdui-text-right">
                            <button class="mdui-btn mdui-color-theme-accent mdui-ripple" onclick="saveInputValue()">
                                保存
                            </button>
                        </div>
                    </div>
                </div>
            </div>
            <div class="mdui-dialog" id="uploadProgress">
                <div class="mdui-dialog-title">修改图片</div>

                <div class="mdui-dialog-content">
                    <div class="mdui-text-center mdui-m-b-1" id="opt">正在获取文件信息，网页可能卡顿...</div>
                    <div class="mdui-progress" style="height: 8px">
                        <div
                            class="mdui-progress-determinate mdui-color-theme-accent"
                            id="progressbar"
                            style="width: 30%"></div>
                    </div>
                    <div class="mdui-typo mdui-m-t-1">
                        <span id="filename">测试文件</span>
                        <span style="float: right"
                            ><span id="nowSize"></span> / <span id="totalSize"></span> (<span id="progresstext"></span>%)</span
                        >
                    </div>
                </div>
            </div>
            <div class="mdui-text-center mdui-typo mdui-m-t-2">
                <div>v1.2.0</div>
                QQ群
                <a
                    target="_blank"
                    href="http://qm.qq.com/cgi-bin/qm/qr?_wv=1027&k=1iqMAY_0neQeXGwQH3CN3SFI9yWTuca3&authKey=kRqedyzi6N%2FHSQcrSa%2BHffJ37pjH%2FpcFT89QZRYEIARKsOcGbZNRLnWWGtATxM%2F%2B&noverify=0&group_code=181989137"
                    >181989137</a
                >
            </div>
            <input type="file" id="fileInput" style="display: none" />
        </div>
        <!-- MDUI JavaScript -->
        <script src="https://cdn.jsdelivr.net/npm/mdui@1.0.2/dist/js/mdui.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@jaames/iro@5"></script>
        <script src="https://cdn.jsdelivr.net/npm/jquery@3.7.0/dist/jquery.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/crc-32@1.2.2/crc32.min.js"></script>

        <script>
            var colorPicker,
                lastTime = 0;
            var CHUNK_SIZE = 1 * 1024 * 1024; // 分片大小，这里设为 2MB
            var currentChunk = 0;
            var file;
            var totalUploaded = 0;
            var totalSize = 0;
            var uploadInProgress = false;
            var uploadMod = "";
            var progressDialog = new mdui.Dialog("#uploadProgress", { modal: true, closeOnEsc: false, history: false });
            var crc32 = 0;
            var chunkList = [];
            var tryTimes = 0;
            // progressDialog.open();
            window.onload = function () {
                colorPicker = new iro.ColorPicker("#color-picker", {
                    width: 200,
                    color: "#fff",
                    layoutDirection: "horizontal",
                    layout: [
                        { component: iro.ui.Wheel, options: {} },
                        { component: iro.ui.Slider, options: { sliderType: "hue" } },
                        { component: iro.ui.Slider, options: { sliderType: "saturation" } },
                        { component: iro.ui.Slider, options: { sliderType: "value" } },
                    ],
                });

                colorPicker.on("input:end", function (color) {
                    document.getElementById("color-show").style.backgroundColor = color.hexString;
                    var str = color.hexString.substr(1, 6);
                    setConfig("color", parseInt(str, 16));
                });
                // setInterval(refreshData, 5000);
                // refreshData();

                mdui.$("#fileInput").on("change", (event) => {
                    console.log(event.target.files[0], uploadInProgress);
                    if (!event.target.files[0] || uploadInProgress) {
                        return;
                    }
                    uploadInProgress = true;
                    $("#filename").text(uploadMod + ".gif");
                    file = event.target.files[0];
                    $("#progressbar").css("width", "0%");
                    progressDialog.open();

                    totalSize = file.size;
                    $("#totalSize").text(formatBytes(totalSize));
                    currentChunk = 0;
                    totalUploaded = 0;
                    fileReader = new FileReader();
                    fileReader.onload = function (e) {
                        byteArray = new Uint8Array(e.target.result);
                        console.log(byteArray);
                        crc32 = CRC32.buf(byteArray);
                        console.log(crc32);
                        $("#opt").text("正在上传文件，请勿关闭网页...");
                        uploadChunk();
                    };
                    fileReader.readAsArrayBuffer(file);
                });
                loadConfig();
            };
            function uploadStart(mod) {
                uploadMod = mod;
                $("#opt").text("正在获取文件信息，网页可能卡顿...");
                document.getElementById("fileInput").value = "";
                document.getElementById("fileInput").click();
            }
            function uploadChunk() {
                var start = currentChunk * CHUNK_SIZE;
                var end = Math.min(file.size, (currentChunk + 1) * CHUNK_SIZE);
                var chunk = file.slice(start, end);
                var crc32_chunk = 0;
                fileReader = new FileReader();
                tryTimes = 0;
                fileReader.onload = function (e) {
                    byteArray = new Uint8Array(e.target.result);
                    console.log(byteArray);
                    crc32_chunk = CRC32.buf(byteArray);
                    console.log(crc32_chunk);
                    doUpload(chunk, crc32_chunk, start, end);
                };
                fileReader.readAsArrayBuffer(chunk);
            }
            function doUpload(chunk, chunkCRC32, start, end) {
                var formData = new FormData();
                formData.append("crc32", chunkCRC32);
                formData.append("chunk", chunk);
                $.ajax({
                    url: "/file/upload", // 修改为服务器端处理上传的URL
                    type: "POST",
                    data: formData,
                    processData: false,
                    contentType: false,
                    dataType: "json",
                    xhr: function () {
                        const xhr = new window.XMLHttpRequest();
                        xhr.upload.addEventListener("progress", function (event) {
                            console.log(event);
                            if (event.lengthComputable) {
                                totalUploaded = currentChunk * CHUNK_SIZE + event.loaded;
                                console.log(totalUploaded, totalSize);
                                var percent = (totalUploaded / totalSize) * 100;
                                $("#progressbar").css("width", parseInt(percent) + "%");
                                $("#progresstext").text(parseInt(percent));
                                $("#nowSize").text(formatBytes(totalUploaded));
                            }
                        });
                        return xhr;
                    },

                    success: function (data) {
                        if (data.code == 1) {
                            currentChunk++;
                            chunkList.push(chunkCRC32);
                            if (end < file.size) {
                                uploadChunk();
                            } else {
                                uploadInProgress = false;
                                $("#opt").text("正在合并上传的文件...");
                                tryTimes = 0;
                                saveFile();
                            }
                        } else {
                            uploadProgressStop();
                            mdui.alert(data.msg);
                        }
                    },
                    error: function (e) {
                        tryTimes++;
                        console.log(e);
                        if (tryTimes > 3) {
                            uploadProgressStop();
                            mdui.alert("文件上传失败，请重试。");
                        } else {
                            doUpload(chunk, chunkCRC32, start, end);
                        }
                    },
                });
            }
            function saveFile() {
                $.ajax({
                    url: "/file/save", // 修改为服务器端处理上传的URL
                    type: "POST",
                    data: { mod: uploadMod, chunks: chunkList.toString() },
                    dataType: "json",
                    contentType: "application/x-www-form-urlencoded; charset=UTF-8",
                    success: function (data) {
                        if (data.code == 1) {
                            $("#opt").text("正在校验文件，请稍后...");
                            checkFile();
                        } else {
                            mdui.alert(data.msg);
                        }
                    },
                    error: function (e) {
                        console.log(e);
                        tryTimes++;
                        if (tryTimes > 3) {
                            uploadProgressStop();
                            mdui.alert("文件合并失败，请重试。");
                        } else {
                            saveFile();
                        }
                    },
                });
            }
            function checkFile() {
                $.ajax({
                    url: "/file/check", // 修改为服务器端处理上传的URL
                    type: "POST",
                    data: { mod: uploadMod, crc32: crc32 },
                    dataType: "json",
                    contentType: "application/x-www-form-urlencoded; charset=UTF-8",
                    success: function (data) {
                        if (data.code == 1) {
                            uploadProgressStop();
                            mdui.alert("文件上传成功，下次显示该界面时生效。");
                        } else {
                            mdui.alert(data.msg);
                        }
                    },
                    error: function (e) {
                        console.log(e);
                        tryTimes++;
                        if (tryTimes > 3) {
                            uploadProgressStop();
                            mdui.alert("文件校验失败，可能是网络波动导致的文件上传不完整。");
                        } else {
                            saveFile();
                        }
                    },
                });
            }
            function uploadProgressStop() {
                uploadInProgress = false;
                progressDialog.close();
            }
            function rgbToHex(color) {
                var in_color = parseInt(color);
                return "#" + in_color.toString(16).padStart(6, "0");
            }
            function formatBytes(bytes, decimals = 2) {
                if (bytes === 0) return "0 Bytes";
                const k = 1024;
                const dm = decimals < 0 ? 0 : decimals;
                const sizes = ["Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"];
                const i = Math.floor(Math.log(bytes) / Math.log(k));
                return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + " " + sizes[i];
            }
            function loadConfig() {
                $.ajax({
                    url: "/config/get", // 修改为服务器端处理上传的URL
                    type: "get",
                    dataType: "json",
                    success: function (data) {
                        if (data.code == 1) {
                            for (const key in data.status) {
                                if (Object.hasOwnProperty.call(data.status, key)) {
                                    const element = data.status[key];
                                    document.getElementById("ico_" + key).style.display = element == false ? "" : "none";
                                }
                            }
                            document.getElementById("input_klipperip").value = data.klipperip;
                            document.getElementById("input_brightness").value = data.brightness;
                            var color = parseInt(data.color);
                            colorPicker.color.hexString = rgbToHex(color);
                            document.getElementById("color-show").style.backgroundColor = rgbToHex(color);
                            mdui.mutation();
                            mdui.updateSliders();
                        } else {
                            mdui.alert("无法获取数据，请与开发人员联系！");
                        }
                    },
                    error: function (e) {
                        mdui.alert("获取数据失败，可能是网络原因导致的.");
                    },
                });
            }
            function setConfig(mod, val) {
                $.ajax({
                    url: "/config/set", // 修改为服务器端处理上传的URL
                    type: "POST",
                    dataType: "json",
                    contentType: "application/x-www-form-urlencoded",
                    data: { mod: mod, val: val },
                    error: function (e) {
                        mdui.alert("数据保存失败，可能是网络异常引起的");
                    },
                });
            }
            function saveInputValue() {
                setConfig("brightness", document.getElementById("input_brightness").value);
                setConfig("klipperip", document.getElementById("input_klipperip").value);
            }
        </script>
    </body>
</html>

)delimiter";