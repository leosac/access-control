Leosac Firmware {#firmware_main}
================================

This is basically a pre-installed SD card with Leosac software running on it.

Todo:
     + Make sure log rotation is properly configured.
     
As an example here is `logrotate` config for `rsyslog`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.conf
/var/log/syslog
{
        rotate 7
        maxsize 10M
        daily
        missingok
        notifempty
        delaycompress
        compress
        postrotate
                invoke-rc.d rsyslog rotate > /dev/null
        endscript
}

/var/log/mail.info
/var/log/mail.warn
/var/log/mail.err
/var/log/mail.log
/var/log/daemon.log
/var/log/kern.log
/var/log/auth.log
/var/log/user.log
/var/log/lpr.log
/var/log/cron.log
/var/log/debug
/var/log/messages
/var/log/leosac.log
{
        rotate 4
        maxsize 10M
        weekly
        missingok
        notifempty
        compress
        delaycompress
        sharedscripts
        postrotate
                invoke-rc.d rsyslog rotate > /dev/null
        endscript
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


When using Jessie which provide `journald` as a logging backend, editing
`/etc/systemd/journald.conf` and setting `MaxSystemUse` should prevent log to grow
too fat.


Disable `ifplugd`. If Leosac is configured to manage the network, `ifplugd` can
cause trouble by reconfiguring the network on its own.
