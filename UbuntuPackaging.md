# Introduction #

Tips about Ubuntu Packaging.

# Details #

Prepare yourself for building packages: http://developer.ubuntu.com/packaging/html/getting-set-up.html (perform all steps excluding tuning bazar)

Firtsly, take from launchpad;
  * urtconnector\_X.X.X-0ubuntu1.debian.tar.gz

Take a fresh sources from git:
```
git clone https://code.google.com/p/urtconnector/
```

Cleanup from .git and pack sources to urtconnector\_X.X.X.orig.tar.gz

Unpack urtconnector\_X.X.X-0ubuntu1.debian.tar.gz to sources root folder (archive contains single folder "debian")

Update all files in debian folder to the new version info (changelog, etc).

After that run:
```
debuild -S
```
This will build package, generating dsc and signed it with your PGP key.

Upload result to the launchpad:
```
dput -f ppa:navrocky/urtconnector-ppa urtconnector_0.9.0-0ubuntu1_source.changes
```

After a several minutes you will receive email notification from the launchpad about status of your uploading. After successfully uploading you can watch building process from the launchpad web interface.

# Ubuntu codenames #

| 11.10 | oneiric |
|:------|:--------|
| 12.04 | precise |