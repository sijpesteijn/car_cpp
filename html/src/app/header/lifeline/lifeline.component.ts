import { Component } from '@angular/core';
import { SettingsService } from '../settings.service';
import { Config } from '../../app.config';
import { Router } from '@angular/router';
import {CarService} from "../../car.service";
import {CameraService} from "../../camera.service";
import {EventService, OBSERVER_ROIS_CLEAR} from "../../event.service";

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
                private router: Router,
                private carService: CarService,
                private eventService: EventService,
                private cameraService: CameraService) {
    }

    ngOnInit() {
        this.startReconnect();
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.router.navigate(['./error/no-lifeline']);
                this.lifeline = new WebSocket(this.config.get('lifeline.ws'));
                clearInterval(this.retry);
                this.lifeline.onopen    = (evt) => {
                    setTimeout(() =>
                    {
                        this.cameraService.loadCameraDimensions();
                        this.cameraService.loadCameraSettings();
                    }, 500);
                    this.eventService.emit(OBSERVER_ROIS_CLEAR);
                    this.router.navigate(['./off']);
                };
                this.lifeline.onmessage = (msg) => {
                    this.carService.updateCar(JSON.parse(msg.data));
                    this.heartbeat = !this.heartbeat;
                    setTimeout(() => this.lifeline.send('moe'), 500);
                };
                this.lifeline.onerror   = (error) => {
                    this.eventService.emit(OBSERVER_ROIS_CLEAR);
                    this.router.navigate(['./error/no-lifeline']);
                };
                this.lifeline.onclose   = (event) => {
                    this.eventService.emit(OBSERVER_ROIS_CLEAR);
                    this.router.navigate(['./error/no-lifeline']);
                    this.startReconnect();
                };
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }
}