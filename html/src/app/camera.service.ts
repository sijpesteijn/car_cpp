
import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Http} from "@angular/http";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";

export interface Dimension {
    name: string;
    width: number;
    height: number;
}

export interface WhiteBalance {
    alpha: number;
    beta: number;
}

export interface CameraSettings {
    dimension?: Dimension;
    captureDelay?: number;
    observersDelay?: number;
    previewDelay?: number;
    whiteBalance?: WhiteBalance;
}

@Injectable()
export class CameraService {
    private cameraSetting: Subject<CameraSettings>;
    private cameraDimensions: Subject<Dimension[]>;

    constructor(private http: Http, private config: Config) {
        this.cameraSetting = new BehaviorSubject(null);
        this.cameraDimensions = new BehaviorSubject(null);
        this.loadCameraSettings();
        this.loadCameraDimensions();
    }

    loadCameraSettings() {
        this.http.get(this.config.get('camera.settings'))
            .subscribe(response => {
                this.cameraSetting.next(response.json());
            });
    }

    loadCameraDimensions() {
        this.http.get(this.config.get('camera.dimensions')).subscribe(response => {
            this.cameraDimensions.next(response.json());
        });
    }

    getCameraInfo(): Observable<CameraSettings> {
        return this.cameraSetting;
    }

    updateCameraInfo(cameraSettings: CameraSettings): Observable<CameraSettings> {
        const json = JSON.stringify(cameraSettings);
        return this.http.post(this.config.get('camera.settings'), json).map(response => {
            const cameraSettings = response.json();
            this.cameraSetting.next(cameraSettings);
            return cameraSettings;
        });
    }

    getDimensions(): Observable<Dimension[]> {
        return this.cameraDimensions;
    }
}