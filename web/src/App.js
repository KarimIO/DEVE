import React, { Component } from 'react';
import MyImages from "./sections/MyImages.js"
import OtherImages from "./sections/OtherImages"
import DownloadedImages from "./sections/DownloadedImages"
import RequestList from "./sections/RequestList.js"
import Credentials from "./sections/Credentials.js"
import FriendsBar from "./sections/FriendsBar.js"
import ImageView from "./sections/ImageView.js"
import ImageUpload from "./sections/ImageUpload.js"
import "./css/main.css"

var interval_delay = 10000;
var page_refresh_interval;
var user_refresh_interval;
var request_refresh_interval;

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
			show_upload: false,
			show_full_img: false,
			full_img: ""
		}
	}
	
	handleErrorsJson = (response) => {
		return new Promise((resolve, reject) => {
			// will resolve or reject depending on status, will pass both "status" and "data" in either case
			let func;
			response.status < 400 ? func = resolve : func = reject;
			response.json().then(data => func(data));
		});
	}
	

	handleErrorsText = (response) => {
		return new Promise((resolve, reject) => {
			// will resolve or reject depending on status, will pass both "status" and "data" in either case
			let func;
			response.status < 400 ? func = resolve : func = reject;
			response.text().then(data => func(data));
		});
	}

	domain = "http://localhost:22000/";

	fetchMyImages = () => {
		let me = this;

		fetch(this.domain + "/images/" + this.state.user.username).then(this.handleErrorsJson)
		.then((e) => {
			me.setState({my_images_loading: false, my_images: e});
		}).catch((e) => {
			alert(e);
			console.log(e);
		});
	}

	fetchOtherImages = (usr = this.state.seluser) => {
		let me = this;

		fetch(this.domain + "/images/" + usr).then(this.handleErrorsJson)
		.then((e) => {
			me.setState({other_images_error: false, other_images_loading: false, other_images: e});
		}).catch((e) => {
			//alert(e);
			console.log(e);
			me.setState({other_images_error: true, other_images_loading: false});
		});
	}

	fetchDownloadedImages = () => {
		let me = this;
		
		fetch(this.domain + "/downloaded").then(this.handleErrorsJson)
		.then((e) => {
			if (e === null) e = [];
			me.setState({downloaded_images_error: false, download_images_loading: false, download_images: e});
		}).catch((e) => {
			alert(e);
			console.log(e);
			me.setState({downloaded_images_error: true, download_images_loading: false});
		});
	}

	fetchUserList = () => {
		let me = this;
		
		fetch(this.domain + "userlist").then(this.handleErrorsJson)
		.then((e) => {
			this.setState({user_list_error: false, user_list_loading: false, user_list: e ? e : []});
		}).catch((e) => {
			alert(e);
			console.log(e);
			me.setState({user_list_error: true, user_list_loading: false});
		});
	}

	fetchRequests = () => {
		let me = this;
		
		fetch(this.domain + "requests").then(this.handleErrorsJson)
		.then((e) => {
			me.setState({req_error: false, req_loading: false, req_list: me.state.req_list.concat(e), ongoing_list: []});
		}).catch((e) => {
			alert(e);
			console.log(e);
			me.setState({req_error: true, req_loading: false});
		});
	}

	setUser = (u) => {
		if (this.state.seluser !== u) {
			// If we're looking at a different user's images, display them.
			this.setState({seluser: u, other_images_loading: true});
			this.setScreen(0);
			this.fetchOtherImages(u);
		}
		else {
			// If not just load the correct subscreen
			this.setScreen(0);
		}
	}

	grantViews = (image, user) => {
		let me = this;

		var views = prompt("How many views would you like to grant?", 0);

		fetch(this.domain + "grant/" + image.ownerID + "/" + image.id + "/" + image.unixTimestamp + "/" + user + "/" + views).then(this.handleErrorsText)
		.then((e) => {
			alert("Views granted.");
		}).catch((e) => {
			alert("Views could not be granted.");
			console.log(e);
		});
	}

	setScreen = (i) => {
		clearInterval(page_refresh_interval);
		if (i === 0) {
			page_refresh_interval = setInterval(this.fetchOtherImages, interval_delay);
			this.fetchOtherImages();
		}
		else if  (i === 1) {
			page_refresh_interval = setInterval(this.fetchMyImages, interval_delay);
			this.fetchMyImages();
		}
		else if (i === 2) {
			page_refresh_interval = setInterval(this.fetchDownloadedImages, interval_delay);
			this.fetchDownloadedImages();
		}
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

		page_refresh_interval = setInterval(this.fetchMyImages, interval_delay);
		this.fetchMyImages();
		user_refresh_interval = setInterval(this.fetchUserList, interval_delay);
		this.fetchUserList();
		request_refresh_interval = setInterval(this.fetchRequests, interval_delay);
		this.fetchRequests();
	}

	handleLogOut = () => {
		this.credentials.logOut();
	}

	removeImage = (data) => {
		let me = this;
		
		fetch(this.domain + "removeimg/" + data.ownerID + "/" + data.id + "/" + data.unixTimestamp).then(this.handleErrorsText)
		.then((e) => {
			alert("Image successfully deleted");
			console.log(e);
			me.fetchMyImages();
		})
		.catch((e) => {
			alert("Could not delete image: " + e);
			console.log(e);
		});
	}

	showImageView = (data) => {
		let me = this;
		
		fetch(this.domain + "image/" + data.ownerID + "/" + data.id + "/" + data.unixTimestamp).then(this.handleErrorsText)
		.then((e) => {
			console.log(e);
			me.setState({show_full_img: true, full_img: e});
			//this.setState({other_images_error: false, other_images_loading: false, other_images: e});
		})
		.catch((e) => {
			alert(e);
			console.log(e);
			//this.setState({other_images_error: true, other_images_loading: false});
		});
	}

	
	requestImg = (data) => {
		let me = this;

		console.log(data);
		
		fetch(this.domain + "requestimg/" + data.ownerID + "/" + data.id + "/" + data.unixTimestamp).then(this.handleErrorsText)
		.then((e) => {
			alert("Request succeeded! Please wait for a response.");
			//this.setState({other_images_error: false, other_images_loading: false, other_images: e});
		})
		.catch((e) => {
			alert("Request failed!");
			console.log(e);
			//this.setState({other_images_error: true, other_images_loading: false});
		});
	}

	hideImageView = () => {
		this.setState({show_full_img: false});
	}

	showImageUpload = () => {
		this.setState({show_upload: true});
	}

	hideImageUpload = () => {
		this.setState({show_upload: false});
	}

	render() {
		let s = this.state.selscreen;
		let notification_count = this.state.req_list.length;
		let has_notifications = notification_count > 0;
			
		return (
			<div className="App">
				<FriendsBar name={this.state.user.username} sel={this.state.selscreen === 0 ? this.state.seluser : ""} setUser={this.setUser} loading={this.state.user_list_loading} list={this.state.user_list} />
				<Credentials ref={instance => { this.credentials = instance; }} domain={this.domain} setUserInfo={this.setUserInfo} />
				<ImageUpload shown={this.state.show_upload} fetchMyImages={this.fetchMyImages} hideImageUpload={this.hideImageUpload} domain={this.domain} />
				<ImageView image={this.state.full_img} shown={this.state.show_full_img} hideImageView={this.hideImageView} />
				<main>
					<nav>
						<span className={s === 1 ? "selected" : ""} onClick={() => this.setScreen(1)}>My Images</span>
						<span className={s === 2 ? "selected" : ""} onClick={() => this.setScreen(2)}>Downloaded Images</span>
						<span className={`${has_notifications ? "request-tab" : ""}${s === 3 ? " selected" : ""}`} onClick={() => this.setScreen(3)}>Requests {has_notifications && <div className="notification">{notification_count}</div>}</span>
						{/*<span className="signout-btn" onClick={this.handleLogOut}>Sign Out</span>*/}
						<span className="add-btn" onClick={this.showImageUpload}></span>
					</nav>
					{s === 0 && <OtherImages error={this.state.other_images_error} requestImg={this.requestImg} showImageView={this.showImageView} loading={this.state.other_images_loading} gallery={this.state.other_images} />}
					{s === 1 && <MyImages removeImage={this.removeImage} showImageView={this.showImageView} loading={this.state.my_images_loading} gallery={this.state.my_images} />}
					{s === 2 && <DownloadedImages showImageView={this.showImageView} loading={this.state.download_images_loading} gallery={this.state.download_images} />}
					{s === 3 && <RequestList grantViews={this.grantViews} showImageView={this.showImageView} loading={this.state.req_loading} ongoing={this.state.ongoing_list} requests={this.state.req_list} />}
				</main>
			</div>
		);
	}
}

export default App;
