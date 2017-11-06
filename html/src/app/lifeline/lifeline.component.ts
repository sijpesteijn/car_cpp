import { Component } from '@angular/core';
import { SettingsService } from '../settings.service';
import { Config } from '../app.config';
import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';
import { Observer } from 'rxjs/Observer';
import { Router } from '@angular/router';

@Component({
    selector: 'lifeline',
    template: require('./lifeline.html'),
    styles  : [require('./lifeline.scss')]
})
export class LifeLineComponent {
    private lifeline: WebSocket;
    private heartbeat: boolean;
    private retry: any;

    constructor(private config: Config,
                private router: Router) {
        // this.create(config.get('lifeline.ws')).subscribe(msg => console.log(msg), error => console.log(error), ()=>
        // console.log('complete'));
    }

    ngAfterViewInit() {
        this.startReconnect();
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.lifeline = new WebSocket(this.config.get('lifeline.ws'));
                clearInterval(this.retry);
                this.lifeline.onopen    = (evt) => {
                    this.router.navigate(['./observers']);
                };
                this.lifeline.onmessage = (msg) => {
                    console.log('msg ', msg);
                    this.heartbeat = !this.heartbeat;
                    setTimeout(() => this.lifeline.send('moe'), 500);
                };
                this.lifeline.onerror   = (error) => {
                    this.router.navigate(['./error/no-lifeline']);
                };
                this.lifeline.onclose   = (event) => {
                    this.router.navigate(['./error/no-lifeline']);
                    this.startReconnect();
                };
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }
}