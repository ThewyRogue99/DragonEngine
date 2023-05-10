import os
import sys

import requests
import json
import time
import urllib

from zipfile import ZipFile

def GetAllGitHubReleases(user, repo):
    responseStr = urllib.request.urlopen(f'https://api.github.com/repos/{user}/{repo}/releases').read().decode()
    return json.loads(responseStr)

def GetLatestGitHubRelease(user, repo):
    return GetAllGitHubReleases(user, repo)[0]

def GetLatestGitHubReleaseByName(user, repo, release_name):
    releases = GetAllGitHubReleases(user, repo)

    for release in releases:
        if release["name"] == release_name:
            return release
    
    raise Exception("Failed to find the latest GitHub release by name")

def DownloadFile(stdscr, url, filepath):
    filepath = os.path.abspath(filepath)
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
            
    if (type(url) is list):
        for url_option in url:
            stdscr.addstr("Downloading\n")
            stdscr.refresh()
            try:
                DownloadFile(stdscr, url_option, filepath)
                return
            except urllib.error.URLError as e:
                stdscr.addstr(f"URL Error encountered: {e.reason}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except urllib.error.HTTPError as e:
                stdscr.addstr(f"HTTP Error  encountered: {e.code}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except:
                stdscr.addstr(f"Something went wrong. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
        raise ValueError(f"Failed to download {filepath}")
    if not(type(url) is str):
        raise TypeError("Argument 'url' must be of type list or string")

    with open(filepath, 'wb') as f:
        response = requests.get(url, stream=True)
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.content)
        else:
            downloaded = 0
            total = int(total)
            startTime = time.time()
            for data in response.iter_content(chunk_size=max(int(total/1000), 1024*1024)):
                downloaded += len(data)
                f.write(data)
                
                try:
                    done = int(50*downloaded/total) if downloaded < total else 50
                    percentage = (downloaded / total) * 100 if downloaded < total else 100
                except ZeroDivisionError:
                    done = 50
                    percentage = 100
                elapsedTime = time.time() - startTime
                try:
                    avgKBPerSecond = (downloaded / 1024) / elapsedTime
                except ZeroDivisionError:
                    avgKBPerSecond = 0.0

                avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
                if (avgKBPerSecond > 1024):
                    avgMBPerSecond = avgKBPerSecond / 1024
                    avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
                stdscr.addstr('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
                stdscr.refresh()

    stdscr.addstr('\n')
    stdscr.refresh()

def UnzipFile(stdscr, filepath, deleteZipFile=True):
    zipFilePath = os.path.abspath(filepath) # get full path of files
    zipFileLocation = os.path.dirname(zipFilePath)

    zipFileContent = dict()
    zipFileContentSize = 0
    with ZipFile(zipFilePath, 'r') as zipFileFolder:
        for name in zipFileFolder.namelist():
            zipFileContent[name] = zipFileFolder.getinfo(name).file_size
        zipFileContentSize = sum(zipFileContent.values())
        extractedContentSize = 0
        startTime = time.time()
        for zippedFileName, zippedFileSize in zipFileContent.items():
            UnzippedFilePath = os.path.abspath(f"{zipFileLocation}/{zippedFileName}")
            os.makedirs(os.path.dirname(UnzippedFilePath), exist_ok=True)
            if os.path.isfile(UnzippedFilePath):
                zipFileContentSize -= zippedFileSize
            else:
                zipFileFolder.extract(zippedFileName, path=zipFileLocation, pwd=None)
                extractedContentSize += zippedFileSize
            try:
                done = int(50*extractedContentSize/zipFileContentSize)
                percentage = (extractedContentSize / zipFileContentSize) * 100
            except ZeroDivisionError:
                done = 50
                percentage = 100
            elapsedTime = time.time() - startTime
            try:
                avgKBPerSecond = (extractedContentSize / 1024) / elapsedTime
            except ZeroDivisionError:
                avgKBPerSecond = 0.0
            avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
            if (avgKBPerSecond > 1024):
                avgMBPerSecond = avgKBPerSecond / 1024
                avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
            stdscr.addstr('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
            stdscr.refresh()

    stdscr.addstr('\n')
    stdscr.refresh()

    if deleteZipFile:
        os.remove(zipFilePath) # delete zip file