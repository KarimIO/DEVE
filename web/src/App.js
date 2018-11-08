import React, { Component } from 'react';
import MyImages from "./sections/MyImages.js"
import OtherImages from "./sections/OtherImages"
import DownloadedImages from "./sections/DownloadedImages"
import RequestList from "./sections/RequestList.js"
import Credentials from "./sections/Credentials.js"
import FriendsBar from "./sections/FriendsBar.js"
import "./css/main.css"

class App extends Component {
	constructor(props) {
		super(props);

		this.state = {
			user: {
				username: "",
				name: ""
			},
			selscreen: 1,
			my_images: [],
			my_images_loading: true,
			download_images: [],
			download_images_loading: true,
			other_images: [],
			other_images_loading: true,
			user_list: [],
			user_list_loading: true,
			req_list: [],
			req_loading: true,
			ongoing_list: [],
			ongoing_loading: true,
		}
	}

	domain = "http://localhost:9090/";

	fetchMyImages = () => {
		fetch(this.domain + "/images/karimah").then((e) => e.json())
		.then((e) => {
			this.setState({my_images_loading: false, my_images: e});
		});
	}

	fetchOtherImages = () => {
		fetch(this.domain + "/images/" + this.state.seluser).then((e) => e.json())
		.then((e) => {
			this.setState({other_images_error: false, other_images_loading: false, other_images: e});
		}).catch((e) => {
			console.log(e);
			this.setState({other_images_error: true, other_images_loading: false});
		});
	}

	fetchDownloadedImages = () => {
		fetch("/server/downloaded.json").then((e) => e.json())
		.then((e) => {
			this.setState({downloaded_images_error: false, download_images_loading: false, download_images: e});
		}).catch((e) => {
			console.log(e);
			this.setState({downloaded_images_error: true, download_images_loading: false});
		});
	}

	fetchUserList = () => {
		fetch(this.domain + "userlist").then((e) => e.json())
		.then((e) => {
			this.setState({user_list_error: false, user_list_loading: false, user_list: e});
		}).catch((e) => {
			console.log(e);
			this.setState({user_list_error: true, user_list_loading: false});
		});
	}

	fetchRequests = () => {
		fetch("/server/requests.json").then((e) => e.json())
		.then((e) => {
			this.setState({req_error: false, req_loading: false, req_list: e.requests, ongoing_list: e.ongoing});
		}).catch((e) => {
			console.log(e);
			this.setState({req_error: true, req_loading: false});
		});
	}

	setUser = (u) => {
		if (this.state.seluser !== u) {
			// If we're looking at a different user's images, display them.
			this.setState({selscreen: 0, seluser: u});
			this.setState({other_images_loading: true});
			setTimeout(this.fetchOtherImages, 1000);
		}
		else {
			// If not just load the correct subscreen
			this.setState({selscreen: 0});
		}
	}

	setScreen = (i) => {
		this.setState({selscreen: i});
	}

	setUserInfo = (user) => {
		console.log("Logged in as: ", user);
		this.setState({user: user});
		
		/*fetch("http://localhost:9080/images/").then(function(response) {
			return response.text();
		}).then(function(data) {
			console.log(data);
			document.body.innerHTML = `<img src='data:image/png;base64, ${data}' />`;
		}).catch((e) => {
			console.log("Error:", e);
		});*/

		setTimeout(this.fetchMyImages, 1000);
		setTimeout(this.fetchDownloadedImages, 1000);
		setTimeout(this.fetchUserList, 400);
		setTimeout(this.fetchRequests, 500);
	}

	handleLogOut = () => {
		this.credentials.logOut();
	}

	render() {
		let s = this.state.selscreen;
		let notification_count = this.state.req_list.length;
		let has_notifications = notification_count > 0;
			
		return (
			<div className="App">
				<FriendsBar name={this.state.user.name} sel={this.state.selscreen === 0 ? this.state.seluser : ""} setUser={this.setUser} loading={this.state.user_list_loading} list={this.state.user_list} />
				<Credentials ref={instance => { this.credentials = instance; }} setUserInfo={this.setUserInfo} />
				<main>
					<nav>
						<span className={s === 1 ? "selected" : ""} onClick={() => this.setScreen(1)}>My Images</span>
						<span className={s === 2 ? "selected" : ""} onClick={() => this.setScreen(2)}>Downloaded Images</span>
						<span className={`${has_notifications ? "request-tab" : ""}${s === 3 ? " selected" : ""}`} onClick={() => this.setScreen(3)}>Requests {has_notifications && <div className="notification">{notification_count}</div>}</span>
						<span className="signout-btn" onClick={this.handleLogOut}>Sign Out</span>
					</nav>
					{s === 0 && <OtherImages loading={this.state.other_images_loading} gallery={this.state.other_images} />}
					{s === 1 && <MyImages loading={this.state.my_images_loading} gallery={this.state.my_images} />}
					{s === 2 && <DownloadedImages loading={this.state.download_images_loading} gallery={this.state.download_images} />}
					{s === 3 && <RequestList loading={this.state.req_loading} ongoing={this.state.ongoing_list} requests={this.state.req_list} />}
				</main>
			</div>
		);
	}
}

export default App;
