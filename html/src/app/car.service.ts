
import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {HttpClient} from "@angular/common/http";

export interface Car {
    throttle: number;
    angle: number;
    mode: number;
    enabled: boolean;
}

@Injectable()
export class CarService {
    private car: Subject<Car>;

    constructor(private http: HttpClient, private config: Config) {
        this.car = new BehaviorSubject({throttle: 0, angle: 0, mode: 0, enabled: false});
    }

    setEngine(throttle: number ) {
        return (this.http.post(this.config.get('car.engine').replace(':throttle', String(throttle)), {})
            .map(response => {
                console.log('Engine: ', response);
                // return response.json();
            }));

    }

    setSteer(angle: number ) {
        return (this.http.post(this.config.get('car.steer').replace(':angle', String(angle)), {})
            .map(response => {
                // console.log('Steer: ', response);
                // return response.json();
            }));

    }

    setCarMode(mode: number) {
        return (this.http.post(this.config.get('car.mode').replace(':mode', String(mode)), {}));
    }

    getCar(): Observable<Car> {
        return this.car;
    }

    updateCar(car: Car) {
        this.car.next(car);
    }
}