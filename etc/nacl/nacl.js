var fileSystem = null;
var localPackages = null;
var remotePackages = null;
var module = null;
var hudArea = null;
var hudText = null;
var hasLoaded = false;
var hasEnded = false;

window.onload = onLoad;

function download(url, type, progressCallback, completionCallback)
{
  var xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.responseType = type;
  xhr.onprogress = progressCallback;
  xhr.onreadystatechange = function () {
    if (xhr.readyState === 4) {
      completionCallback(xhr.response);
    }
  };
  xhr.send();
}

function readLocalManifest()
{
  fileSystem.root.getFile('/cache/manifest.json', {}, function (fileEntry) {
    fileEntry.file(function (file) {
      var reader = new FileReader();
      reader.onloadend = function () {
        localPackages = JSON.parse(this.result);
        fetchRemoteManifest();
      };
      reader.readAsText(file);
    });
  }, function () {
    localPackages = {};
    fetchRemoteManifest();
  });
}

function fetchRemoteManifest()
{
  download('manifest.json', 'text', function () {
  }, function (data) {
    remotePackages = JSON.parse(data);
    fileSystem.root.getFile('/cache/manifest.json', {create: true}, function (fileEntry) {
      fileEntry.createWriter(function (writer) {
        writer.onwriteend = deleteOrphans;
        writer.write(new Blob([data], {type: 'text'}));
      });
    });
  }, function () {
    console.error('Failed to load manifest');
  });
}

// Delete unlisted files.
function deleteOrphans()
{
  fileSystem.root.getDirectory('/cache', {}, function (directoryEntry) {
    directoryEntry.createReader().readEntries(function (entries) {
      var nextEntry = function (i) {
        if (i === entries.length) {
          updatePackages();
        }
        else {
          var fileEntry = entries[i];
          if (fileEntry.name in remotePackages || fileEntry.name === 'manifest.json') {
            nextEntry(i + 1);
          }
          else {
            console.log('deleting ' + fileEntry.fullPath);
            fileEntry.remove(function () {
              nextEntry(i + 1);
            });
          }
        }
      };
      nextEntry(0);
    });
  });
}

function updatePackages()
{
  var nextEntry = function (i) {
    var pkgNames = Object.keys(remotePackages);
    if (i === pkgNames.length) {
      module.postMessage({'oz': navigator.language});
      hasLoaded = true;
    }
    else {
      var pkg = pkgNames[i];
      if (pkg in localPackages && localPackages[pkg] === remotePackages[pkg]) {
        console.log(pkg + ' up to date');
        nextEntry(i + 1);
      }
      else {
        download(pkg, 'blob', function (e) {
          hudText.innerHTML = MSG_DOWNLOADING + '<br>' + pkg;

          if (e.lengthComputable && e.total > 0) {
            var percent = Math.round(e.loaded / e.total * 100.0);
            hudText.innerHTML += ' ' + percent + ' %';
          }
        }, function (data) {
          fileSystem.root.getFile('/cache/' + pkg, {create: true}, function (fileEntry) {
            fileEntry.createWriter(function (writer) {
              writer.onwriteend = function () {
                console.log(pkg + ' downloaded');
                nextEntry(i + 1);
              };
              writer.write(data);
            });
          });
        });
      }
    }
  };
  nextEntry(0);
}

function onMessage(e)
{
  var message = e.data;

  if (message === "init") {
    hudText.innerHTML = MSG_INITIALISING;
  }
  else if (message === 'none') {
    hudArea.style.display = 'none';
    hudText.innerHTML = '';
  }
  else if (message.substring(0, 4) === 'http') {
    window.open(text, '_blank');
  }
  else if (message === 'quit') {
    hasEnded = true;

    module.style.visibility = 'hidden';
    hudArea.style.display = 'inline-table';
    hudText.innerHTML = MSG_FINISHED;
  }
}

function onProgress()
{
  if (hasLoaded) {
    hudText.innerHTML = MSG_LOADING;

    if (event.lengthComputable && event.total > 0) {
      var percent = Math.round(event.loaded / event.total * 100.0);
      hudText.innerHTML += '<br>' + percent + ' %';
    }
  }
}

function onLoadEnd()
{
  hudText.innerHTML = MSG_LAUNCHING;
}

function onLoad()
{
  module = document.getElementById('module');
  hudArea = document.getElementById('hudArea');
  hudText = document.getElementById('hudText');

  module.addEventListener('message', onMessage, true);
  module.addEventListener('progress', onProgress, true);
  module.addEventListener('loadend', onLoadEnd, true);

  window.webkitRequestFileSystem(TEMPORARY, 0, function (fs) {
    fileSystem = fs;
    fileSystem.root.getDirectory('/cache', {create: true}, function () {
      readLocalManifest();
    });
  });
}
